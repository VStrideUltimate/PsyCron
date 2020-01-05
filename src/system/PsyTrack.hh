//
// PsyCron
// File: PsyTrack.hh
// Author: Dylan R. Wagner (drw6528@rit.edu)
// Description:
// 		PsyTrack header file.
//

#ifndef PSYTRACK_HH
#define PSYTRACK_HH

#include "PsyRail.hh"
#include "UIIL.hh"
#include "Codes.hh"
#include "Macros.hh"

namespace psycron {

class PsyCron;

void* psyalloc_key_func(size_t size);

class PsyTrackBase{

friend class PsyCron;

public:

	PsyTrackBase(PsyCron* os, uint16_t id) :
		m_hold_os{os},
		m_id{id}
		{}

	uint16_t m_id;

	UIIL& get_user_parameters();

protected:

	PsyCron* m_hold_os;

private:

	virtual void execute() = 0;

};

template <typename EnvType>
class PsyTrack final: public PsyTrackBase{

public:

	struct PriorityRoutineArgs{
		PriorityRoutineArgs(
			PriorityRoutine<EnvType>* routine,
			uint16_t id,
			uint16_t value,
			bool is_active = true
		) 
			: m_routine{routine}
			, m_id{id}
			, m_value{value}
			, m_is_active{is_active}
		{};

		PriorityRoutine<EnvType>* m_routine;
		uint16_t m_id;
		uint16_t m_value;
		bool m_is_active;
	};

	struct TimedRoutineArgs{
		TimedRoutineArgs(
			TimedRoutine<EnvType>* routine,
			uint16_t id,
			uint32_t value,
			bool is_active = true
		) 
			: m_routine{routine}
			, m_id{id}
			, m_value{value}
			, m_is_active{is_active}
		{};

		TimedRoutine<EnvType>* m_routine;
		uint16_t m_id;
		uint32_t m_value;
		bool m_is_active;
	};

	template<typename ... Args>
	PsyTrack(
		uint16_t id, 
		EnvType&& global_env, 
		PsyCron* os,
		Args ... args
	)
	  : PsyTrackBase{os, id}
	  ,	m_global_env{global_env}
	  , m_priority_rail{this, count_args(args...).priority_num}
	  , m_timed_rail{this, count_args(args...).timed_num}
	{
		insert_routine(args...);
	}

	inline void *operator new(size_t size){
        return psyalloc_key_func(size);
    };

	EnvType& get_environment(){
		return m_global_env;
	};

	/**
	 * Attempts to activate the specifed routine in fist the priority rail, then
	 * the timed rail.
	 * 
	 * @param id The id of the routine to be activated.
	 * @return True on success, false on failure.
	 */
	bool activate_routine(uint16_t id){
		if(m_priority_rail.activate_rail_routine(id)) return true;
		if(m_timed_rail.activate_routine(id)) return true;

		return false;
	}

	/**
	 * Attempts to deactivate the specifed routine in fist the priority rail, then
	 * the timed rail.
	 * 
	 * @param id The id of the routine to be activated.
	 * @return True on success, false on failure.
	 */
	bool deactivate_routine(uint16_t id){
		if(m_priority_rail.deactivate_rail_routine(id)) return true;
		if(m_timed_rail.deactivate_routine(id)) return true;

		return false;
	}

private:

	struct ArgCount{

		ArgCount(uint16_t a_priority_num, uint16_t a_timed_num)
			: priority_num{a_priority_num}
			, timed_num(a_timed_num)
		{}

		uint16_t priority_num;
		uint16_t timed_num;
	};

	void execute(){
		// @TODO Do priority, timed rail swapping
		m_priority_rail.execute();
	};

	void insert_routine(PriorityRoutineArgs arg){
		m_priority_rail.insert_routine(arg.m_routine, arg.m_id, arg.m_value, arg.m_is_active);
	};

	template <typename ... RoutineArgs>
	void insert_routine(PriorityRoutineArgs arg, RoutineArgs ... args){
		m_priority_rail.insert_routine(arg.m_routine, arg.m_id, arg.m_value, arg.m_is_active);
		insert_routine(args...);
	};

	void insert_routine(TimedRoutineArgs arg){
		EASSERT_ABORT(!this->get_user_parameters().sys_milli_second, errMILLI_SECOND_USER_CONFIG_MISSING);
		m_timed_rail.insert_routine(arg.m_routine, arg.m_id, arg.m_value, arg.m_is_active);
	};

	template <typename ... RoutineArgs>
	void insert_routine(TimedRoutineArgs arg, RoutineArgs ... args){
		m_timed_rail.insert_routine(arg.m_routine, arg.m_id, arg.m_value, arg.m_is_active);
		insert_routine(args...);
	};

	ArgCount count_args(PsyTrack<EnvType>::PriorityRoutineArgs arg){
		return ArgCount{1, 0};
	};

	ArgCount count_args(PsyTrack<EnvType>::TimedRoutineArgs arg){
		return ArgCount{0, 1};
	};

	template<typename ... Args>
	ArgCount count_args(PsyTrack<EnvType>::PriorityRoutineArgs arg, Args...args){
		ArgCount result = count_args(args...);
		result.priority_num += 1;

		return result;
	};

	template<typename ... Args>
	ArgCount count_args(PsyTrack<EnvType>::TimedRoutineArgs arg, Args...args){
		ArgCount result = count_args(args...);
		result.timed_num += 1;

		return result;
	};

	EnvType m_global_env;

	// The priority rail which regular routines will be placed in.
	PriorityPsyRail<EnvType> m_priority_rail;

	// The timed rail which timed routines will be placed in.
	TimedPsyRail<EnvType> m_timed_rail;
};

}

#endif
