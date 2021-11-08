/**
* By:Mahya Shahmohammadimehrjardi
*/

#ifndef __CodeController_HPP__
#define __CodeController_HPP__

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

///#include "../data_structures/message.hpp"
using namespace cadmium;
using namespace std;

//Port definition
    struct CodeCont_defs {
        //Output ports
        struct out_a : public out_port<bool> { };
        struct out_b : public out_port<bool> { };
        struct out_c : public out_port<bool> { };
        
        
        //Input ports
        struct inpc : public in_port<int> { };
        struct inpr : public in_port<int> { };
        struct inpo : public in_port<bool> { };

  };



    template<typename TIME>
    class Code_Cont {
        using defs=CodeCont_defs; // putting definitions in context
        public:


            
            // default constructor
            Code_Cont() noexcept{

              state.type = 0;
              state.inpc = 0;
              state.inpr  = 0;
              state.inpo  = false;
              
              
             
            }
            
 // state definition
            struct state_type{
              int type;
              int inpc;
              int inpr;
              bool inpo;
              bool out_a;
              bool out_b;
              bool out_c;
              
            }; 
            state_type state;            
           
            // ports definition
            
            using input_ports=std::tuple<typename defs::inpc, typename defs::inpr , typename defs::inpo>;
            using output_ports=std::tuple<typename defs::out_a, typename defs::out_b, typename defs::out_c>;

            // internal transition
            void internal_transition() {
            ////state== ifnotvalid
		    if (state.type==3){
		         state.type=1;         
		    } 
            }

            // external transition
            void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs) { 
            
		    for(const auto &x : get_messages<typename CodeCont_defs::inpc>(mbs)) {
			    if(state.type==0)	
			    	state.inpc=x;
		    	
		    
		    }
		    for(const auto &y : get_messages<typename CodeCont_defs::inpr>(mbs)) {
		    	state.inpr=y;
		    	
		    
		    }           
		    for(const auto &z : get_messages<typename CodeCont_defs::inpo>(mbs)) {
		    	state.inpo=z;

		    }

		    if(state.type==1){
		    	if(state.inpr==state.inpc){
		    		state.type=2;
		    	}
		    	else{
		    		state.type=3;
		    	}
		    	
		    	if(state.inpo==1){
		    		state.type=4;
		    	}
		    }

		    if(state.type==0 && state.inpo==1){
		    	
	    		state.type=4;
		    	
		    }            
                        
            
              if(state.type==4){
              	state.out_a=false;
              	state.out_b=false;
              
              	state.out_c=true;
              }
              else if(state.type==2){
              	state.out_a=true;
              	state.out_b=false;
              
              	state.out_c=false;
              }
              else if(state.type==3){
              	state.out_a=false;
              	state.out_b=true;
              
              	state.out_c=false;
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
              get_messages<typename defs::out_c>(bags).push_back(state.out_c);              
              return bags;
            }


	    // time_advance function
	    TIME time_advance() const {
			
	      
		return TIME("00:00:01");
	    }


            friend std::ostringstream& operator<<(std::ostringstream& os, const typename Code_Cont<TIME>::state_type& i) {
              os << "Current state: " << i.type; 
              return os;
            }
        };     


#endif // __CodeController_HPP__
