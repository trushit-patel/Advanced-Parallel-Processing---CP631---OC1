// program tries all passwords of certain length until it finds one that
// produces the searched for hash value
//
// compile with
// gcc -O2 find.c -lcrypt -o find.x

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
  double start_time = MPI_Wtime();
  /* password hash (with salt), for hashes below password has 3 characters  */
//  const char *const hash = "$1$WicCakKA$vKI5prjBi7KzmR3maGrza1";
//  const char *const hash = "$1$zkcCa.NA$s7K1PdYGSTF5xUngmCAHM.";
//  const char *const hash = "$1$FlcCaENA$CQ2dmLurILPF3AqEMG2rt/";
//  const char *const hash = "$1$SlcCaRNA$zZbCNhZytQGwArj1dhL7U0";

  /* password hash (with salt), for hashes below password has 4 characters  */
// const char *const hash = "$1$JO/EajqE$TZIldQ3Bc/E83rDfV0lCs1";
 const char *const hash = "$1$mL.Ja9Ru$H4G643/p73.C3nW9O9FES.";

  int num_pass_chars=64; //number of characters allowed in password
  int pass_len=4; // length of password
  char passwd[pass_len+1];
  const char *const passchars =  //characters allowed in password
    "./0123456789ABCDEFGHIJKLMNOPQRST"
    "UVWXYZabcdefghijklmnopqrstuvwxyz";

  long long int number_of_possible_passwords;
  long long int ilong,itest;
  int j;

  char *result;
  int ok;

  int p;
  int my_rank;
  int local_number_of_passwords;
  int local_flag = 0;
  int global_flag = 0;
  
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

// compute total number of possible passwords
  number_of_possible_passwords=1;
  for(j=0;j<pass_len;j++) number_of_possible_passwords*=num_pass_chars;

  local_number_of_passwords = number_of_possible_passwords / p;


// loop through all possible passords
  for(ilong = local_number_of_passwords * my_rank; ilong < local_number_of_passwords * (my_rank + 1); ilong++){

    itest=ilong;
    for(j=0;j<pass_len;j++){
      passwd[j]=passchars[itest%num_pass_chars];
      itest=itest/num_pass_chars;
    }
    passwd[j]='\0'; //add null character to terminate the string

    result = crypt(passwd, hash);

    /* Test the result. */
    local_flag = strcmp (result, hash) == 0;


    MPI_Allreduce(&local_flag, &global_flag, 1, MPI_INT, MPI_LOR, MPI_COMM_WORLD);

    if(local_flag){
        printf(" %s is the password \n", passwd);
        printf("Password found exiting process %d .\n", my_rank);
        break;
    }
    
    if (global_flag) {
        //printf("Process %d exiting because another process found the password.\n", my_rank);
        break;
    }
    
  }
  
  double end_time = MPI_Wtime();
  if (my_rank == 0) {
        printf("Total execution time: %f seconds\n", end_time - start_time);
  }

  MPI_Finalize();
  return 0;
}

