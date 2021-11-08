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
#include "../atomics/CodeController.hpp"

//C++ libraries
#include <iostream>
#include <string>

using namespace std;
using namespace cadmium;
using namespace cadmium::basic_models::pdevs;

#define INPUT_FILEPATH1 "../input_data/inputcodee.txt"
#define INPUT_FILEPATH2 "../input_data/openrequestt.txt"
#define INPUT_FILEPATH3 "../input_data/emergopenn.txt"
#define OUTPUT_FILEPATH "../results/CodeController_test_output_validcode.txt"
#define OUTPUT_FILEPATH "../results/CodeController_test_output_incre.txt"
#define OUTPUT_FILEPATH "../results/CodeController_test_output_emerg.txt"

using TIME = NDTime;



/***** Define output ports for coupled model *****/
struct outvalid: public out_port<bool>{};
struct outincre: public out_port<bool>{};
struct outemerg: public out_port<bool>{};


/****** Input Reader atomic model declaration *******************/
template<typename T>
class InputReader_int : 
public iestream_input<int,T> {
    public:
        InputReader_int () = default;
        InputReader_int (const char* file_path) : iestream_input<int,T>(file_path) {}
};

template<typename T>
class InputReader_bool : 
public iestream_input<bool,T> {
    public:
        InputReader_bool () = default;
        InputReader_bool (const char* file_path) : iestream_input<bool,T>(file_path) {}
};


int main(){

    /****** Input Reader for Time Checker atomic model instantiation *******************/
    const char * i_input_data_inputcodee = INPUT_FILEPATH1;
    shared_ptr<dynamic::modeling::model> input_reader_inputcodee;
    input_reader_inputcodee = dynamic::translate::make_dynamic_atomic_model<InputReader_int, TIME, const char*>("input_reader_inputcodee", move(i_input_data_inputcodee));

    /****** Input Reader for Plagiarism Checker atomic model instantiation *******************/
    const char * i_input_data_openrequestt = INPUT_FILEPATH2;
    shared_ptr<dynamic::modeling::model> input_reader_openrequestt;
    input_reader_openrequestt = dynamic::translate::make_dynamic_atomic_model<InputReader_int, TIME, const char*>("input_reader_openrequestt", move(i_input_data_openrequestt));

    /****** Input Reader for Mail Checker atomic model instantiation *******************/
    const char * i_input_data_emergopenn = INPUT_FILEPATH3;
    shared_ptr<dynamic::modeling::model> input_reader_emergopenn;
    input_reader_emergopenn = dynamic::translate::make_dynamic_atomic_model<InputReader_bool, TIME, const char*>("input_reader_emergopenn", move(i_input_data_emergopenn));

    /****** Evaluator atomic model instantiation *******************/
    shared_ptr<dynamic::modeling::model> codecontroller1;
    codecontroller1 = dynamic::translate::make_dynamic_atomic_model<Code_Cont, TIME>("codecontroller1");

    /*******TOP MODEL********/
    dynamic::modeling::Ports iports_TOP;
    iports_TOP = {};
    dynamic::modeling::Ports oports_TOP;
    oports_TOP = {typeid(outvalid), typeid(outincre), typeid(outemerg)};
    dynamic::modeling::Models submodels_TOP;
    submodels_TOP = {input_reader_inputcodee, input_reader_openrequestt, input_reader_emergopenn, codecontroller1};
    dynamic::modeling::EICs eics_TOP;
    eics_TOP = {};
    dynamic::modeling::EOCs eocs_TOP;
    eocs_TOP = {
        dynamic::translate::make_EOC<CodeCont_defs::out_a, outvalid>("codecontroller1"),
        dynamic::translate::make_EOC<CodeCont_defs::out_b, outincre>("codecontroller1"),
        dynamic::translate::make_EOC<CodeCont_defs::out_c, outemerg>("codecontroller1"),
    };
    dynamic::modeling::ICs ics_TOP;
    ics_TOP = {
        dynamic::translate::make_IC<iestream_input_defs<int>::out,CodeCont_defs::inpc>("input_reader_inputcodee","codecontroller1"),
        dynamic::translate::make_IC<iestream_input_defs<int>::out,CodeCont_defs::inpr>("input_reader_openrequestt","codecontroller1"),
        dynamic::translate::make_IC<iestream_input_defs<bool>::out,CodeCont_defs::inpo>("input_reader_emergopenn","codecontroller1")
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
    static ofstream out_state("../results/CodeController_test_output_state.txt");
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
