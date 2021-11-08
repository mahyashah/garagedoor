/**
* By: Mahya Shahmohammadimehrjardi
*/



#ifndef __AlarmGenerator_HPP__
#define __AlarmGenerator_HPP__


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>

#include <limits>
#include <assert.h>
#include <string>
#include <random>

using namespace cadmium;
using namespace std;

//Port definition
    struct AlarmGen_defs {
        //Output ports
        struct out_a: public out_port<bool> { };
        struct out_b : public out_port<bool> { };
        
        //Input ports
        struct inpe : public in_port<bool> { };
        struct inpa : public in_port<bool> { };
        struct inpu : public in_port<bool> { };
  };

    template<typename TIME>
    class Alarm_Gen {
        using defs=AlarmGen_defs; // putting definitions in context
        public:
            //Parameters to be overwriten when instantiating the atomic model
            
            // default constructor
            Alarm_Gen() noexcept{
              state.type = 0;
              state.inpe= false;
              state.inpa= false;
              state.inpu= false;
            }
            
            // state definition
            struct state_type{
              int type;
              bool inpe;
              bool inpa;
              bool inpu;
              bool out_a;
              bool out_b;
            }; 
            state_type state;
            // ports definition
            
            using input_ports=std::tuple<typename defs::inpe, typename defs::inpa, typename defs::inpu>;
            using output_ports=std::tuple<typename defs::out_a, typename defs::out_b>;

            // internal transition
            void internal_transition() {
            
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
               
              for(const auto &x : get_messages<typename defs::inpe>(mbs))
                state.inpe = x;
                   
              for(const auto &x : get_messages<typename defs::inpa>(mbs))
                state.inpa= x;
                
              for(const auto &x : get_messages<typename defs::inpu>(mbs))
                state.inpu = x;
                    
              if(state.type==0){
              	if(state.inpu){
              		state.type=1;
              	}
              	else{
              		state.type=0;
              	}
              	
              }
              
              if(state.type==1){
              	if(state.inpa){
              		state.type=0;
              		
              	}
              	else{
              		state.type=1;
              	}
              }
              
              if(state.inpe){
              	state.type==2;
              }
              if(state.type==1){
              	state.out_a=true;
              	state.out_b=false;
              	
              }
              else if(state.type==2){
              	state.out_b=true;
              	state.out_a=false;
              	
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
              

              get_messages<typename defs::out_a>(bags).push_back(state.out_a);
              get_messages<typename defs::out_b>(bags).push_back(state.out_b);
              
             
              return bags;
            }

            // time_advance function
            TIME time_advance() const { 

              return std::numeric_limits<TIME>::infinity();
            }

            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Alarm_Gen<TIME>::state_type& i) {
              os << "Current state: " << i.type; 
              return os;
            }
        };     


#endif //  __AlarmGenerator_HPP__
