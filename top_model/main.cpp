/**
* Mahya Shah
*101213253
* Carleton University
*/


//Cadmium Simulator headers
#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/dynamic_model.hpp>
#include <cadmium/modeling/dynamic_model_translator.hpp>
#include <cadmium/engine/pdevs_dynamic_runner.hpp>
#include <cadmium/logger/common_loggers.hpp>

//Time class header
#include <NDTime.hpp>


//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/CodeController.hpp"
#include "../atomics/DoorTimerr.hpp"
#include "../atomics/Counterr.hpp"
#include "../atomics/AlarmGenerator.hpp"
#include "../atomics/IndoorLightening.hpp"
#include "../atomics/OutdoorLightening.hpp"

//C++ headers
#include <iostream>
#include <chrono>
#include <algorithm>
#include <string>


using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

using TIME = NDTime;
/********************************/
// Define input port for coupled models 
/********************************/
/*******DoorController*********/
struct input_code : public cadmium::in_port<int>{};
struct open_request : public cadmium::in_port<int>{};
struct emerg_open : public cadmium::in_port<bool>{};
/*******LIGHTALARMCONTROLLER*********/
struct alarm_reset : public cadmium::in_port<bool>{};
struct motion : public cadmium::in_port<bool>{};
struct light : public cadmium::in_port<float>{};
struct light_valid_code : public cadmium::in_port<bool>{};
struct alarm_emerg : public cadmium::in_port<bool>{};
struct alarm_unauth_attempt : public cadmium::in_port<bool>{};

/********************************/
// Define output ports for coupled model 
/********************************/
/*******DoorController*********/
struct door_open : public cadmium::out_port<bool>{};
struct door_unauth_attempt : public cadmium::out_port<bool>{};
struct door_emerg : public cadmium::out_port<bool>{};
struct door_valid_code : public cadmium::out_port<bool>{};
/*******LIGHTALARMCONTROLLER*********/
struct minor_alarm : public cadmium::out_port<bool>{};
struct major_alarm : public cadmium::out_port<bool>{};
struct inlon : public cadmium::out_port<bool>{};
struct outlon : public cadmium::out_port<bool>{};

/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_Int : public iestream_input<float,T> {
public:
    InputReader_Int() = default;
    InputReader_Int(const char* file_path) : iestream_input<float,T>(file_path) {}
};

int main(int argc, char ** argv) {

    if (argc < 2) {
        cout << "Program used with wrong parameters. The program must be invoked as follow:";
        cout << argv[0] << " path to the input file " << endl;
        return 1; 
    }
    /****** Input Reader atomic model instantiation *******************/
    string input = argv[1];
    const char * i_input = input.c_str();
    shared_ptr<dynamic::modeling::model> input_reader = dynamic::translate::make_dynamic_atomic_model<InputReader_Int, TIME, const char* >("input_reader" , move(i_input));

    shared_ptr<dynamic::modeling::model> CodeController = dynamic::translate::make_dynamic_atomic_model<Code_Cont, TIME>("CodeController");
    shared_ptr<dynamic::modeling::model> DoorTimerr = dynamic::translate::make_dynamic_atomic_model<Door_Timer, TIME>("DoorTimerr");
    shared_ptr<dynamic::modeling::model> Counterr = dynamic::translate::make_dynamic_atomic_model<Counter_C, TIME>("Counterr");
    shared_ptr<dynamic::modeling::model> AlarmGenerator = dynamic::translate::make_dynamic_atomic_model<Alarm_Gen, TIME>("AlarmGenerator");
    shared_ptr<dynamic::modeling::model> IndoorLightening = dynamic::translate::make_dynamic_atomic_model<Light_in, TIME>("IndoorLightening");
    shared_ptr<dynamic::modeling::model> OutdoorLightening = dynamic::translate::make_dynamic_atomic_model<Light_out, TIME>("OutdoorLightening");
/********************************/
/*******DoorController*********/
/********************************/
    dynamic::modeling::Ports iports_DOOR_CONTROLLER = {typeid(input_code), typeid(open_request), typeid(emerg_open)};
    dynamic::modeling::Ports oports_DOOR_CONTROLLER = {typeid(door_open), typeid(door_unauth_attempt), typeid(door_emerg), typeid(door_valid_code)};
    dynamic::modeling::Models submodels_DOOR_CONTROLLER =  {CodeController, DoorTimerr, Counterr};
    
    dynamic::modeling::EICs eics_DOOR_CONTROLLER = {

    cadmium::dynamic::translate::make_EIC<input_code, CodeCont_defs::inpc>("CodeController"),
    cadmium::dynamic::translate::make_EIC<open_request, CodeCont_defs::inpr>("CodeController"),
    cadmium::dynamic::translate::make_EIC<emerg_open, CodeCont_defs::inpo>("CodeController"),     
  };
    dynamic::modeling::EOCs eocs_DOOR_CONTROLLER = {
     cadmium::dynamic::translate::make_EOC<DoorTimer_defs::out, door_open>("door_open"),
     cadmium::dynamic::translate::make_EOC<Counter_defs::out,door_unauth_attempt >("door_unauth_attempt"),
     cadmium::dynamic::translate::make_EOC<CodeCont_defs::out_c, door_emerg>("door_emerg"),
     cadmium::dynamic::translate::make_EOC<CodeCont_defs::out_a, door_valid_code>("door_valid_code")
  };
    dynamic::modeling::ICs ics_DOOR_CONTROLLER = {
     cadmium::dynamic::translate::make_IC<CodeCont_defs::out_a, DoorTimer_defs::inp>("CodeController","DoorTimerr"),
     cadmium::dynamic::translate::make_IC<CodeCont_defs::out_b, Counter_defs::inp>("CodeController","Counterr")
   
  };
    shared_ptr<dynamic::modeling::coupled<TIME>> DOOR_CONTROLLER;
 

DOOR_CONTROLLER=make_shared<dynamic::modeling::coupled<TIME>>(
        "DOOR_CONTROLLER", submodels_DOOR_CONTROLLER, iports_DOOR_CONTROLLER, oports_DOOR_CONTROLLER, eics_DOOR_CONTROLLER, eocs_DOOR_CONTROLLER, ics_DOOR_CONTROLLER
    );

   /********************************/
/*******LIGHTALARMCONTROLLER*********/
/********************************/
    dynamic::modeling::Ports iports_LIGHTALARM_CONTROLLER = {typeid(alarm_reset), typeid(motion), typeid(light), typeid(alarm_unauth_attempt), typeid(alarm_emerg), typeid(light_valid_code)};
    dynamic::modeling::Ports oports_LIGHTALARM_CONTROLLER = {typeid(major_alarm), typeid(minor_alarm), typeid(outlon), typeid(inlon)};
    dynamic::modeling::Models submodels_LIGHTALARM_CONTROLLER =  {AlarmGenerator, IndoorLightening, OutdoorLightening};
    
   dynamic::modeling::EICs eics_LIGHTALARM_CONTROLLER = {

    cadmium::dynamic::translate::make_EIC<alarm_reset, AlarmGen_defs::inpa>("AlarmGenerator"),
    cadmium::dynamic::translate::make_EIC<motion, Lightout_defs::input_m>("OutdoorLightening"),
    cadmium::dynamic::translate::make_EIC<light, Lightout_defs::input_l>("OutdoorLightening")    
  };
   dynamic::modeling::EOCs eocs_LIGHTALARM_CONTROLLER = {
     cadmium::dynamic::translate::make_EOC<AlarmGen_defs::out_a, minor_alarm>("AlarmGenerator"),
     cadmium::dynamic::translate::make_EOC<AlarmGen_defs::out_b, major_alarm>("AlarmGenerator"),
     cadmium::dynamic::translate::make_EOC<Lightin_defs::out, inlon>("IndoorLightening"),
     cadmium::dynamic::translate::make_EOC<Lightout_defs::out, outlon>("OutdoorLightening")
  };
   dynamic::modeling::ICs ics_LIGHTALARM_CONTROLLER = {
   
  }; 
  shared_ptr<dynamic::modeling::coupled<TIME>> LIGHTALARM_CONTROLLER;
    LIGHTALARM_CONTROLLER = make_shared<dynamic::modeling::coupled<TIME>>(
        "LIGHTALARM_CONTROLLER", submodels_LIGHTALARM_CONTROLLER, iports_LIGHTALARM_CONTROLLER, oports_LIGHTALARM_CONTROLLER, eics_LIGHTALARM_CONTROLLER, eocs_LIGHTALARM_CONTROLLER, ics_LIGHTALARM_CONTROLLER
    );
    
/*************************/
/*******GARAGETOP MODEL*********/
/*************************/
     dynamic::modeling::Ports iports_GARAGETOP = {typeid(motion), typeid(light), typeid(input_code), typeid(open_request), typeid(emerg_open), typeid(alarm_reset)};
     dynamic::modeling::Ports oports_GARAGETOP = {typeid(door_open), typeid(minor_alarm), typeid(major_alarm), typeid(inlon), typeid(outlon)};
  
     dynamic::modeling::Models submodels_GARAGETOP =  { DOOR_CONTROLLER, LIGHTALARM_CONTROLLER};
  
     dynamic::modeling::EICs eics_GARAGETOP = {
     cadmium::dynamic::translate::make_EIC<motion, Lightout_defs::input_m>("LIGHTALARM_CONTROLLER"),
     cadmium::dynamic::translate::make_EIC<light, Lightout_defs::input_l>("LIGHTALARM_CONTROLLER"),
     cadmium::dynamic::translate::make_EIC<alarm_reset, AlarmGen_defs::inpa>("LIGHTALARM_CONTROLLER"),
     cadmium::dynamic::translate::make_EIC<input_code, CodeCont_defs::inpc>("DOOR_CONTROLLER"),
     cadmium::dynamic::translate::make_EIC<emerg_open, CodeCont_defs::inpo>("DOOR_CONTROLLER"),
     cadmium::dynamic::translate::make_EIC<open_request, CodeCont_defs::inpr>("DOOR_CONTROLLER")
     };
     dynamic::modeling::EOCs eocs_GARAGETOP = {
     dynamic::translate::make_EOC<DoorTimer_defs::out,door_open>("DOOR_CONTROLLER"),
     dynamic::translate::make_EOC<AlarmGen_defs::out_a,minor_alarm>("LIGHTALARM_CONTROLLER"),
     dynamic::translate::make_EOC<AlarmGen_defs::out_b,major_alarm>("LIGHTALARM_CONTROLLER"),
     dynamic::translate::make_EOC<Lightin_defs::out,inlon>("LIGHTALARM_CONTROLLER"),
     dynamic::translate::make_EOC<Lightout_defs::out,outlon>("LIGHTALARM_CONTROLLER")

     };
     dynamic::modeling::ICs ics_GARAGETOP = {



     dynamic::translate::make_IC<door_valid_code, light_valid_code>("DOOR_CONTROLLER", "LIGHTALARM_CONTROLLER"),
     dynamic::translate::make_IC<door_emerg, alarm_emerg>("DOOR_CONTROLLER", "LIGHTALARM_CONTROLLER"), 
     dynamic::translate::make_IC<door_unauth_attempt, alarm_unauth_attempt>("DOOR_CONTROLLER", "LIGHTALARM_CONTROLLER")       
  };
    
    
    shared_ptr<dynamic::modeling::coupled<TIME>> GARAGETOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "GARAGETOP", submodels_GARAGETOP, iports_GARAGETOP, oports_GARAGETOP, eics_GARAGETOP, eocs_GARAGETOP, ics_GARAGETOP 
    );
///////////////////////////////////////////////

	 /*******TOP COUPLED MODEL********/
    dynamic::modeling::Ports iports_TOP = {};
    dynamic::modeling::Ports oports_TOP = {typeid(door_open), typeid(minor_alarm), typeid(major_alarm), typeid(inlon), typeid(outlon)};
    dynamic::modeling::Models submodels_TOP = {input_reader, GARAGETOP};
    dynamic::modeling::EICs eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP = {
        dynamic::translate::make_EOC<door_open,door_open>("GARAGETOP"),
        dynamic::translate::make_EOC<minor_alarm,minor_alarm>("GARAGETOP"),
        dynamic::translate::make_EOC<major_alarm,major_alarm>("GARAGETOP"),
        dynamic::translate::make_EOC<inlon,inlon>("GARAGETOP"),
        dynamic::translate::make_EOC<outlon,outlon>("GARAGETOP")
    };
    dynamic::modeling::ICs ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<bool>::out, motion >("input_reader","GARAGETOP"),
        dynamic::translate::make_IC<iestream_input_defs<float>::out, light>("input_reader","GARAGETOP"),
        dynamic::translate::make_IC<iestream_input_defs<bool>::out, alarm_reset>("input_reader","GARAGETOP"),
        dynamic::translate::make_IC<iestream_input_defs<int>::out, input_code>("input_reader","GARAGETOP"),
        dynamic::translate::make_IC<iestream_input_defs<int>::out, open_request>("input_reader","GARAGETOP"),
        dynamic::translate::make_IC<iestream_input_defs<bool>::out, emerg_open>("input_reader","GARAGETOP")
    };
    shared_ptr<cadmium::dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages("../simulation_results/GARAGETOP_output_messages.txt");
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../simulation_results/GARAGETOP_output_state.txt");
    struct oss_sink_state{
        static ostream& sink(){          
            return out_state;
        }
    };
    
    using state=logger::logger<logger::logger_state, dynamic::logger::formatter<TIME>, oss_sink_state>;
    using log_messages=logger::logger<logger::logger_messages, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_mes=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_messages>;
    using global_time_sta=logger::logger<logger::logger_global_time, dynamic::logger::formatter<TIME>, oss_sink_state>;

    using logger_top=logger::multilogger<state, log_messages, global_time_mes, global_time_sta>;

    /************** Runner call ************************/ 
    dynamic::engine::runner<NDTime, logger_top> r(TOP, {0});
    r.run_until(NDTime("00:10:00:000"));
    return 0;
    }
