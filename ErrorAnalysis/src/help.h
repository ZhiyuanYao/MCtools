/* ===  FUNCTION  ======================================================================
 *  Description: The helper message for --help flag
 * ====================================================================================*/
void help(void) {
    printf("\nThis program is intended to calculate the expectation value and standard deviation"
            "\nof a physical variable Q where Q is a function of underlying direct obserservales."
            "\nBy direct obserservales, we mean the observable O that appear in simulation such"
            "\nthat <O> = (O_nu1 + O_nu2 + ...) / Z_nu. Refer to documentation for detailed reason."
            "\n\n\n"
            "===========\n"
            " Usage : \n"
            "===========\n"
            "1. Define the number of variables, N_VAR of the Q as a function of direct observables\n"
            "   Os and the function Q = func(O1, O2, ...). Note for numerical efficiency, we can also\n"
            "   define a const coefficient assosicated with Q. For example, \n\n"
            "                          Cv = (<E2> - <E>^2) / (T*T) \n \n"
            "   we define N_VAR =2, func() as var[0] - var[1]*var[1] and COEFF as 1.0 / (T * T). Here\n"
            "   var[0] corresponds to E2, var[1] corresponds to E.\n \n"
            "2. The correspondance between var[] and Os are defined in DataInfos[] with explanation of \n"
            "   items of DataStruc in the source code parameter.h. The sequence of data defined there also\n"
            "   determines the correspondance with a particular var[i]. For the particular example above, \n"
            "   we need to set \n\n"
            "                     DataInfos[] = { {1, {1}, pathE2}, {1, {1}, pathE} } \n\n"
            "   where E2, E are stored as first column in pathE2, pathE respectively. If E, E2 are stored \n"
            "   first, second column in a single file pathE_E2, the setting of DataInfos[] should be \n\n"
            "                     DataInfos[] = { {2, {2, 1}, pathE_E2} } \n\n"
            "   where the sequence {2, 1} defines var[0] to be E2, and var[1] as E. Although the alternative \n\n"
            "                     DataInfos[] = { {1, {2}, pathE_E2}, {1, {1}, pathE_E2} } \n\n"
            "   also works. We recommend the first method which is concise and effcient in loading the data.\n"
          );

    printf("\n\nEnter to proceed to parameter.h.\n");
    getchar();
    system("vim parameter.h");
    exit(1);
}
