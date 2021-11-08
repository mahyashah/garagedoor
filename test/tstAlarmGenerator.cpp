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

//Messages structures
//#include "../data_structures/message.hpp"

//Atomic model headers
#include <cadmium/basic_model/pdevs/iestream.hpp> //Atomic model for inputs
#include "../atomics/AlarmGenerator.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

#define INPUT_FILEPATH1 "../input_data/Alarm_reset.txt"
#define INPUT_FILEPATH2 "../input_data/emerg.txt"
#define INPUT_FILEPATH3 "../input_data/unauth.txt"
#define OUTPUT_FILEPATH "../results/AlarmGenerator_test_output_minor_alarm.txt"
#define OUTPUT_FILEPATH "../results/AlarmGenerator_test_output_major_alarm.txt"


using TIME = NDTime;



/***** Define output ports for coupled model *****/
struct outminor: public out_port<bool>{};
struct outmajor: public out_port<bool>{};



/****** Input Reader atomic model declaration *******************/

template<typename T>
class InputReader_bool : 
public iestream_input<bool,T> {
    public:
        InputReader_bool () = default;
        InputReader_bool (const char* file_path) : iestream_input<bool,T>(file_path) {}
};


int main(){

    /****** Input Reader for Time Checker atomic model instantiation *******************/
    const char * i_input_data_inputemerg = INPUT_FILEPATH1;
    shared_ptr<dynamic::modeling::model> input_reader_inputemerg;
    input_reader_inputemerg = dynamic::translate::make_dynamic_atomic_model<InputReader_bool, TIME, const char*>("input_reader_inputemerg", move(i_input_data_inputemerg));

    /****** Input Reader for Plagiarism Checker atomic model instantiation *******************/
    const char * i_input_data_inputalarm = INPUT_FILEPATH2;
    shared_ptr<dynamic::modeling::model> input_reader_inputalarm;
    input_reader_inputalarm = dynamic::translate::make_dynamic_atomic_model<InputReader_bool, TIME, const char*>("input_reader_inputalarm", move(i_input_data_inputalarm));

    /****** Input Reader for Mail Checker atomic model instantiation *******************/
    const char * i_input_data_inputunauth = INPUT_FILEPATH3;
    shared_ptr<dynamic::modeling::model> input_reader_inputunauth;
    input_reader_inputunauth = dynamic::translate::make_dynamic_atomic_model<InputReader_bool, TIME, const char*>("input_reader_inputunauth", move(i_input_data_inputunauth));

    /****** Evaluator atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> AlarmGenerator1;
    AlarmGenerator1 = dynamic::translate::make_dynamic_atomic_model<Alarm_Gen, TIME>("AlarmGenerator1");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};
    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(outminor), typeid(outmajor)};
    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_reader_inputemerg, input_reader_inputalarm, input_reader_inputunauth, AlarmGenerator1};
    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
        dynamic::translate::make_EOC<AlarmGen_defs::out_a, outminor>("AlarmGenerator1"),
        dynamic::translate::make_EOC<AlarmGen_defs::out_b, outmajor>("AlarmGenerator1"),

    };
    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<bool>::out,AlarmGen_defs::inpe>("input_reader_inputemerg","AlarmGenerator1"),
        dynamic::translate::make_IC<iestream_input_defs<bool>::out,AlarmGen_defs::inpa>("input_reader_inputalarm","AlarmGenerator1"),
        dynamic::translate::make_IC<iestream_input_defs<bool>::out,AlarmGen_defs::inpu>("input_reader_inputunauth","AlarmGenerator1")
    };
    

    shared_ptr<dynamic::modeling::coupled<TIME>> TOP;
    TOP = make_shared<dynamic::modeling::coupled<TIME>>(
        "TOP", submodels_TOP, iports_TOP, oports_TOP, eics_TOP, eocs_TOP, ics_TOP 
    );

    /*************** Loggers *******************/
    static ofstream out_messages(OUTPUT_FILEPATH);
    struct oss_sink_messages{
        static ostream& sink(){          
            return out_messages;
        }
    };
    static ofstream out_state("../results/AlarmGenerator_test_output_state.txt");
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
    r.run_until(NDTime("04:00:00:000"));
    



    cout<<"Evaluator Model Run Successful."<<"\n";
    cout<<"To see the Output, go to results folder."<<"\n";
	return 0;
}
