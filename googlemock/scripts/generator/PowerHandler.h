
#ifndef API_POW_HPP
#define API_POW_HPP

#include <binder/IServiceManager.h>

#include <utils/Timer.h>
#include <utils/Handler.h>
#include <utils/SLLooper.h>
#include <utils/Vector.h>
#include <utils/Mutex.h>

// #include "services/PowerManagerService/PowerManager.h"
#include <services/PowerManagerService/PowerManager.h>

#include "pal/power.hpp"
//#include "PowerHandler.hpp"

#define PAL_XID_POW  3000000

// @CGA_VARIANT_START{"powerHandler.hpp:__GLOBAL_HEAD__:__YOUR_CODE__:variant"}
    // add your code
// @CGA_VARIANT___END{"powerHandler.hpp:__GLOBAL_HEAD__:__YOUR_CODE__:variant"}



namespace pal {
    namespace pow {

class PowerHandler : public sl::Handler
{
public:

    // Destructor
    virtual ~PowerHandler();
    static PowerHandler* instance();

    static void cancel_work(pal::xact_id_t xid);

    //Sync API (no Cb)
    //--void cmd_handshake(pal::regid_t id, pal::pow::handshake_response rsp);
    //--pal::api_status cmd_request_power_lock(pal::pow::lock_id& id);
    //--pal::api_status cmd_release_power_lock(pal::pow::lock_id id);
	virtual pal::api_status release_power_lock( 
		pal::pow::lock_id id
		);
	virtual pal::api_status request_power_lock( pal::pow::lock_id & id ,  pal::pow::lock_type type);

    //handler
    virtual void handleMessage(const sp<sl::Message>& msg);
    virtual void dongle(const sp<sl::Message>& msg);

	void handshake( regid_t id ,  handshake_response rsp);

private:
    // Constructor
    PowerHandler(android::sp<sl::SLLooper>& looper);


    int32_t getLockIdOfXId(pal::xact_id_t xid);
    void waitLockFree(pal::xact_id_t xid);

    // for PowerManager
    // NO TIDL manager : power
    //--void onNotifyPowerState(const int32_t newState, const int32_t reason);
    //--void onNotifyWakeup(const int32_t currentState, const int32_t reason);
    //--void onNotifyPowerMode(const int32_t newMode);
    void onErrControlPower(const int32_t err_reason, const int32_t errPowerID, const int32_t currPowerID);
         // onErrControlPower(const int32_t err_reason, const int32_t errPowerID, const int32_t currPowerID) function is implemented in PowerYoursInputManager.cpp file.
    void onPowerModeChanged(const int32_t newMode);
         // onPowerModeChanged(const int32_t newMode) function is implemented in PowerYoursInputManager.cpp file.
    int32_t onPowerPreHandler(const int32_t toState, const int32_t reason);
         // onPowerPreHandler(const int32_t toState, const int32_t reason) function is implemented in PowerYoursInputManager.cpp file.
    void onPowerStateChanged(const int32_t newState, const int32_t reason);
         // onPowerStateChanged(const int32_t newState, const int32_t reason) function is implemented in PowerYoursInputManager.cpp file.
    void onWakeup(const int32_t currentState, const int32_t reason);
         // onWakeup(const int32_t currentState, const int32_t reason) function is implemented in PowerYoursInputManager.cpp file.

    class PowerBnPowerStateReceiver : public BnPowerStateReceiver
    {
    public:
        /**
         * @brief This is the constructor function for the powerReceiver.
         *
         * @param[in] handler     android PowerHandler
         * @retval   void
         *
         */
        PowerBnPowerStateReceiver(PowerHandler handler) : mHandler(handler){}

        /**
         * @brief This is the virtual destructor function.
         *
         * @retval   void
         *
         */
        virtual ~PowerBnPowerStateReceiver() {}  // charles : add destructor in all services

        virtual void onErrControlPower(const int32_t err_reason, const int32_t errPowerID, const int32_t currPowerID){
				// getArgWithCommaFromFunctionDeclare( "onErrControlPower(const int32_t err_reason, const int32_t errPowerID, const int32_t currPowerID)" )
				mHandler.onErrControlPower( err_reason , errPowerID , currPowerID );
		}

        virtual void onPowerModeChanged(const int32_t newMode){
				// getArgWithCommaFromFunctionDeclare( "onPowerModeChanged(const int32_t newMode)" )
				mHandler.onPowerModeChanged( newMode );
		}

        virtual int32_t onPowerPreHandler(const int32_t toState, const int32_t reason){
			return 
				// getArgWithCommaFromFunctionDeclare( "onPowerPreHandler(const int32_t toState, const int32_t reason)" )
				mHandler.onPowerPreHandler( toState , reason );
		}

        virtual void onPowerStateChanged(const int32_t newState, const int32_t reason){
				// getArgWithCommaFromFunctionDeclare( "onPowerStateChanged(const int32_t newState, const int32_t reason)" )
				mHandler.onPowerStateChanged( newState , reason );
		}

        virtual void onWakeup(const int32_t currentState, const int32_t reason){
				// getArgWithCommaFromFunctionDeclare( "onWakeup(const int32_t currentState, const int32_t reason)" )
				mHandler.onWakeup( currentState , reason );
		}


    private:
        PowerHandler &mHandler;
    };



public:

	//static const int32_t CMD_REGISTER_STATE = 1;
       
	static const int32_t CMD_GET_STATE = 1;   
	static const int32_t CMD_HANDSHAKE = 2;   
	static const int32_t CMD_REGISTER_STATE = 3;   
	static const int32_t CMD_REGISTER_STATE_WITH_HANDSHAKE = 4;   
	static const int32_t CMD_RELEASE_POWER_LOCK = 5;   
	static const int32_t CMD_REQUEST_POWER_LOCK = 6;   
	static const int32_t CMD_SCHEDULE_WAKEUP = 7;   
	static const int32_t CMD_UNREGISTER_STATE = 8;   

    enum {
	    LOCK_ID_BASE = 0,
		LOCK_ID_GET_STATE,
		LOCK_ID_HANDSHAKE,
		LOCK_ID_REGISTER_STATE,
		LOCK_ID_REGISTER_STATE_WITH_HANDSHAKE,
		LOCK_ID_RELEASE_POWER_LOCK,
		LOCK_ID_REQUEST_POWER_LOCK,
		LOCK_ID_SCHEDULE_WAKEUP,
		LOCK_ID_UNREGISTER_STATE,
	    LOCK_ID_MAX
    };


private:
    // sp<IPowerStateReceiver> mPowerReceiver;
	// BnPowerStateReceiver
	android::sp<IPowerStateReceiver> mIPowerStateReceiver;

    //pal::pow::state_cb      mPowerStateCB;
	// question : does it change? we need one copy.
		// return callback : (state_cb) m_get_state.callback  : std::function< void(state power_state, const meta_data &md, api_status result_code)>
	struct struct_get_state_t* pget_state_cb;
		// return callback : (state_reg_cb) m_register_state.reg_callback  : std::function< void(api_status result_code, regid_t registration_id)>
		// real (register) callback : (state_cb) m_register_state.power_callback : std::function< void(state power_state, const meta_data &md, api_status result_code)>
	struct struct_register_state_t* pregister_state_cb;
		// return callback : (state_reg_cb) m_register_state_with_handshake.reg_callback  : std::function< void(api_status result_code, regid_t registration_id)>
		// real (register) callback : (state_cb) m_register_state_with_handshake.power_callback : std::function< void(state power_state, const meta_data &md, api_status result_code)>
	struct struct_register_state_with_handshake_t* pregister_state_with_handshake_cb;
		// return callback : (api_result_cb) m_schedule_wakeup.callback  : std::function< void(api_status result_code)>
	struct struct_schedule_wakeup_t* pschedule_wakeup_cb;
		// return callback : (api_result_cb) m_unregister_state.reg_callback  : std::function< void(api_status result_code)>
	struct struct_unregister_state_t* punregister_state_cb;

    //pal::pow::state mPowerState;
	// question : does it change? if you need it. you can add __YOUR_CODE__

    //pal::pow::meta_data mMetaData;
	// question : does it changan add __YOUR_CODE__?

    mutable android::Mutex mCBLock[LOCK_ID_MAX];

	// @CGA_VARIANT_START{"power:PowerHandler:__CLASS_TAIL__:__YOUR_CODE__:variant"}
// public: 
    // add your code
// private: 
    // add your code
	// @CGA_VARIANT___END{"power:PowerHandler:__CLASS_TAIL__:__YOUR_CODE__:variant"}
};


// Class that defines the API active state.
extern const std::string getEnumName(api_active v);
// Enum that defines the return status of all API function calls.
extern const std::string getEnumName(api_status v);
// Enum that defines the handshake response values.
extern const std::string getEnumName(handshake_response v);
// Enum that defines the types of power locks that GMS can request from the platform.
extern const std::string getEnumName(lock_type v);
// Enum that defines the current module power mode.
extern const std::string getEnumName(mode v);
// Enum that defines the power source is this module running on.
extern const std::string getEnumName(power_source v);
// Enum that defines the report frequency type of CAN signals.
extern const std::string getEnumName(report_freq v);
// Enum that defines the power states the ECU can be in.
extern const std::string getEnumName(state v);

extern void print_api_return( std::string prefix , api_return& v);
extern void print_meta_data( std::string prefix , meta_data& v);

extern std::string print_api_return_buf( std::string prefix , api_return& v);
extern std::string print_meta_data_buf( std::string prefix , meta_data& v);

// @CGA_VARIANT_START{"powerHandler.hpp:__GLOBAL_TAIL__:__YOUR_CODE__:variant"}
    // add your code
// @CGA_VARIANT___END{"powerHandler.hpp:__GLOBAL_TAILHEAD__:__YOUR_CODE__:variant"}

    }
}


#endif  // API_POW_HPP
