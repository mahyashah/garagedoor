/**
* By: Mahya Shahmohammadimehrjardi
*/

#ifndef __OutdoorLightening_HPP__
#define __OutdoorLightening_HPP__

#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>
#include <math.h> 
#include <assert.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>


using namespace cadmium;
using namespace std;

//Port definition
    struct Lightout_defs {
        //Output ports
        struct out : public out_port<bool> { };
        
        //Input ports
        struct input_m : public in_port<bool> { };
        struct input_l : public in_port<float> { };
  };

    template<typename TIME>
    class Light_out {
        using defs=Lightout_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            
            // default constructor
            Light_out() noexcept{
              state.type = false;
              state.input_m = false;
              state.input_l = false;

            }
            
            // state definition
            struct state_type{
              int type;
              bool input_m;
              bool input_l;
         
            }; 
            state_type state;
            // ports definition
            
            using input_ports=std::tuple<typename defs::input_m, typename defs::input_l>;
            using output_ports=std::tuple<typename defs::out>;

            // internal transition
            void internal_transition() {
            	if(state.type==1){
            		state.type=0;
            	}

            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
               
              for(const auto &x : get_messages<typename defs::input_m>(mbs))
              	state.input_m=x;

              for(const auto &x : get_messages<typename defs::input_l>(mbs))
                state.input_l = x>0;
                
                if(state.type==0){
                	if(state.input_l){
                		state.type=0;
                	}
                	else
                		state.type=2;
                	
                }	
                if(state.type==2){
                	if(state.input_m){
                		state.type=1;
                	}
                	else
                		state.type=0;	
                
                }
                

            }

            // confluence transition
            void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs) {
              internal_transition();
              external_transition(TIME(), std::move(mbs));
            }

            // output function
            typename make_message_bags<output_ports>::type output() const {
              typename make_message_bags<output_ports>::type bags;
              get_messages<typename defs::out>(bags).push_back(state.type==1);
              return bags;
            }

            // time_advance function
            TIME time_advance() const { 
              if(state.type)
                return TIME("00:00:10");
              return std::numeric_limits<TIME>::infinity();
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Light_out<TIME>::state_type& i) {
              os << "Current state: " << i.type; 
              return os;
            }
        };     


#endif // __OutdoorLightening_HPP__
