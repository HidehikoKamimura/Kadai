#include <stdlib.h>
#include "KseFileIO/KseCsvReader.h"
#include "KseMath/KseVector.h"
#include "KseMath/KseStatistics.h"

int main(int argc, char* argv[])
{
    if (argc < 3) return -1;
    
    int idx_y = atoi(argv[2]);
    KseCsvReader<double> csv_rdr(argv[1]);

    KseVector<double>* pXVec;
    KseVector<double> YVec;
    long nVec = csv_rdr.Read_LinReg("\t", idx_y, pXVec, YVec);
    
    KseLinearRegression<double> lnreg;
    lnreg.SetSample(pXVec, nVec);

    KseVector<double> beta_hat;
    lnreg.EstimateBeta(YVec, beta_hat);
    
    printf("[result]\n");
    for (unsigned long i = 0UL; i < beta_hat.m_nDim; i++) {
        printf("beta_hat[%ld] = %.3f\n", i, beta_hat.m_pCoord[i]);
    }
    printf("\n");

    delete[] pXVec;

    return 0;
}