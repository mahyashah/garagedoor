# garagedoor
**************************************************
* Garagedoor System
* 
*
* Mahya Shah (Carleton ID 101213253)
* 
*
*
**************************************************
This model contains total of 6 atomic models, two coupled model that creats the top model which are

To run the model, 

 1) Open bash prompt and type the command 'make clean; make all'
 
 That will create all the executables in the folder 'bin'

 Once the executables files are generated;

  2) Open bash prompt in 'bin' folder and type the following command to run the AlarmGenerator atomic model;

./AlarmGenerator_TEST
 
  Similary, you can run the other atomic models this way

  all the output files will be created inside the "Simulation results" folder

  The Input data files are created such that it contains all the test cases needed to check the performance of the model. 
  ****************************************************************
 After checking the etomic models one by one
To run the top model you need to clear the valid_code, incre, emerg and unaurh test files in the inputs 
****************************************************************

For atomic models; all the .HPP files are in the folder 'atomics' and .CPP files are in folder 'tests'
.CPP file of the Top model is in folder 'top_model'


************************************************************
Model is described in details in the Assignment.docx file
