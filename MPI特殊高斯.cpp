﻿# include <sys/time.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <mpi.h>
#include <pmmintrin.h>
#include <omp.h>
using namespace std;
static const int thread_count = 4;
const int Num = 1000;
const int pasNum = 5000;
const int lieNum = 4000;
unsigned int Act[lieNum][Num] = { 0 };
unsigned int Pas[pasNum][Num] = { 0 };
void init_A()
{
    unsigned int a;
    ifstream infile("消元子.txt");
    char fin[10000] = { 0 };
    int index;
    while (infile.getline(fin, sizeof(fin)))
    {
        std::stringstream line(fin);
        int biaoji = 0;
        while (line >> a)
        {
            if (biaoji == 0)
            {
                index = a;
                biaoji = 1;
            }
            int k = a % 32;
            int j = a / 32;

            int temp = 1 << k;
            Act[index][Num - 1 - j] += temp;
            Act[index][Num] = 1;
        }
    }
}

void init_P()
{
    unsigned int a;
    ifstream infile("被消元行.txt");
    char fin[10000] = { 0 };
    int index = 0;
    while (infile.getline(fin, sizeof(fin)))
    {
        std::stringstream line(fin);
        int biaoji = 0;

        while (line >> a)
        {
            if (biaoji == 0)
            {
                Pas[index][Num] = a;
                biaoji = 1;
            }

            int k = a % 32;
            int j = a / 32;

            int temp = 1 << k;
            Pas[index][Num - 1 - j] += temp;
        }
        index++;
    }
}

void f_ordinary()
{
    timeval t_start;
    timeval t_end;
    gettimeofday(&t_start, NULL);

    bool sign;
    do
    {
        int i;
        for (i = lieNum - 1; i - 8 >= -1; i -= 8)
        {
            for (int j = 0; j < pasNum; j++)
            {
                while (Pas[j][Num] <= i && Pas[j][Num] >= i - 7)
                {
                    int index = Pas[j][Num];

                    if (Act[index][Num] == 1)
                    {
                        for (int k = 0; k < Num; k++)
                        {
                            Pas[j][k] = Pas[j][k] ^ Act[index][k];
                        }
                        int num = 0, S_num = 0;
                        for (num = 0; num < Num; num++)
                        {
                            if (Pas[j][num] != 0)
                            {
                                unsigned int temp = Pas[j][num];
                                while (temp != 0)
                                {
                                    temp = temp >> 1;
                                    S_num++;
                                }
                                S_num += num * 32;
                                break;
                            }
                        }
                        Pas[j][Num] = S_num - 1;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        for (i = i + 8; i >= 0; i--)
        {
            for (int j = 0; j < pasNum; j++)
            {
                while (Pas[j][Num] == i)
                {
                    if (Act[i][Num] == 1)
                    {
                        for (int k = 0; k < Num; k++)
                        {
                            Pas[j][k] = Pas[j][k] ^ Act[i][k];
                        }
                        int num = 0, S_num = 0;
                        for (num = 0; num < Num; num++)
                        {
                            if (Pas[j][num] != 0)
                            {
                                unsigned int temp = Pas[j][num];
                                while (temp != 0)
                                {
                                    temp = temp >> 1;
                                    S_num++;
                                }
                                S_num += num * 32;
                                break;
                            }
                        }
                        Pas[j][Num] = S_num - 1;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        sign = false;
        for (int i = 0; i < pasNum; i++)
        {
            int temp = Pas[i][Num];
            if (temp == -1)
            {
                continue;
            }
            if (Act[temp][Num] == 0)
            {
                for (int k = 0; k < Num; k++)
                    Act[temp][k] = Pas[i][k];
                Pas[i][Num] = -1;
                sign = true;
            }
        }

    } while (sign == true);
    gettimeofday(&t_end, NULL);
    cout << "ordinary time cost: "
        << 1000 * (t_end.tv_sec - t_start.tv_sec) +
        0.001 * (t_end.tv_usec - t_start.tv_usec) << "ms" << endl;
}


void f_ordinary1()
{
    timeval t_start;
    timeval t_end;
    gettimeofday(&t_start, NULL);

    int i;
    for (i = lieNum - 1; i - 8 >= -1; i -= 8)
    {
        for (int j = 0; j < pasNum; j++)
        {
            while (Pas[j][Num] <= i && Pas[j][Num] >= i - 7)
            {
                int index = Pas[j][Num];
                if (Act[index][Num] == 1)
                {
                    for (int k = 0; k < Num; k++)
                    {
                        Pas[j][k] = Pas[j][k] ^ Act[index][k];
                    }
                    int num = 0, S_num = 0;
                    for (num = 0; num < Num; num++)
                    {
                        if (Pas[j][num] != 0)
                        {
                            unsigned int temp = Pas[j][num];
                            while (temp != 0)
                            {
                                temp = temp >> 1;
                                S_num++;
                            }
                            S_num += num * 32;
                            break;
                        }
                    }
                    Pas[j][Num] = S_num - 1;

                }
                else
                {
                    for (int k = 0; k < Num; k++)
                        Act[index][k] = Pas[j][k];

                    Act[index][Num] = 1;
                    break;
                }

            }
        }
    }


    for (int i = lieNum % 8 - 1; i >= 0; i--)
    {
        for (int j = 0; j < pasNum; j++)
        {
            while (Pas[j][Num] == i)
            {
                if (Act[i][Num] == 1)
                {
                    for (int k = 0; k < Num; k++)
                    {
                        Pas[j][k] = Pas[j][k] ^ Act[i][k];
                    }
                    int num = 0, S_num = 0;
                    for (num = 0; num < Num; num++)
                    {
                        if (Pas[j][num] != 0)
                        {
                            unsigned int temp = Pas[j][num];
                            while (temp != 0)
                            {
                                temp = temp >> 1;
                                S_num++;
                            }
                            S_num += num * 32;
                            break;
                        }
                    }
                    Pas[j][Num] = S_num - 1;

                }
                else
                {
                    for (int k = 0; k < Num; k++)
                        Act[i][k] = Pas[j][k];

                    Act[i][Num] = 1;
                    break;
                }
            }
        }
    }


    gettimeofday(&t_end, NULL);
    cout << "ordinary time cost: "
        << 1000 * (t_end.tv_sec - t_start.tv_sec) +
        0.001 * (t_end.tv_usec - t_start.tv_usec) << "ms" << endl;
}


void super(int rank, int num_proc)
{

    int i;

#pragma omp parallel num_threads(thread_count) 
    for (i = lieNum - 1; i - 8 >= -1; i -= 8)
    {
#pragma omp for schedule(dynamic,20)     
        for (int j = 0; j < pasNum; j++)
        {
            if (int(j % num_proc) == rank)
            {

                while (Pas[j][Num] <= i && Pas[j][Num] >= i - 7)
                {
                    int index = Pas[j][Num];

                    if (Act[index][Num] == 1)
                    {

                        int k;
                        __m128 va_Pas, va_Act;
                        for (k = 0; k + 4 <= Num; k += 4)
                        {
                            va_Pas = _mm_loadu_ps((float*)&(Pas[j][k]));
                            va_Act = _mm_loadu_ps((float*)&(Act[index][k]));

                            va_Pas = _mm_xor_ps(va_Pas, va_Act);
                            _mm_store_ss((float*)&(Pas[j][k]), va_Pas);
                        }

                        for (; k < Num; k++)
                        {
                            Pas[j][k] = Pas[j][k] ^ Act[index][k];
                        }

                        int num = 0, S_num = 0;
                        for (num = 0; num < Num; num++)
                        {
                            if (Pas[j][num] != 0)
                            {
                                unsigned int temp = Pas[j][num];
                                while (temp != 0)
                                {
                                    temp = temp >> 1;
                                    S_num++;
                                }
                                S_num += num * 32;
                                break;
                            }
                        }
                        Pas[j][Num] = S_num - 1;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

#pragma omp parallel num_threads(thread_count) 
    for (int i = lieNum % 8 - 1; i >= 0; i--)
    {
#pragma omp for schedule(dynamic,20)
        for (int j = 0; j < pasNum; j++)
        {
            if (int(j % num_proc) == rank)
            {
                while (Pas[j][Num] == i)
                {
                    if (Act[i][Num] == 1)
                    {

                        int k;
                        __m128 va_Pas, va_Act;
                        for (k = 0; k + 4 <= Num; k += 4)
                        {
                            va_Pas = _mm_loadu_ps((float*)&(Pas[j][k]));
                            va_Act = _mm_loadu_ps((float*)&(Act[i][k]));

                            va_Pas = _mm_xor_ps(va_Pas, va_Act);
                            _mm_store_ss((float*)&(Pas[j][k]), va_Pas);
                        }

                        for (; k < Num; k++)
                        {
                            Pas[j][k] = Pas[j][k] ^ Act[i][k];
                        }
                        int num = 0, S_num = 0;
                        for (num = 0; num < Num; num++)
                        {
                            if (Pas[j][num] != 0)
                            {
                                unsigned int temp = Pas[j][num];
                                while (temp != 0)
                                {
                                    temp = temp >> 1;
                                    S_num++;
                                }
                                S_num += num * 32;
                                break;
                            }
                        }
                        Pas[j][Num] = S_num - 1;

                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }

}
void f_mpi()
{
    int num_proc;
    int rank;

    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        timeval t_start;
        timeval t_end;

        gettimeofday(&t_start, NULL);
        int sign;
        do
        {

            for (int i = 0; i < pasNum; i++)
            {
                int flag = i % num_proc;
                if (flag == rank)
                    continue;
                else
                    MPI_Send(&Pas[i], Num + 1, MPI_FLOAT, flag, 0, MPI_COMM_WORLD);
            }
            super(rank, num_proc);

            for (int i = 0; i < pasNum; i++)
            {
                int flag = i % num_proc;
                if (flag == rank)
                    continue;
                else
                    MPI_Recv(&Pas[i], Num + 1, MPI_FLOAT, flag, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }


            sign = 0;
            for (int i = 0; i < pasNum; i++)
            {
                int temp = Pas[i][Num];
                if (temp == -1)
                {
                    continue;
                }

                if (Act[temp][Num] == 0)
                {
                    for (int k = 0; k < Num; k++)
                        Act[temp][k] = Pas[i][k];
                    Pas[i][Num] = -1;
                    sign = 1;
                }
            }

            for (int r = 1; r < num_proc; r++)
            {
                MPI_Send(&sign, 1, MPI_INT, r, 2, MPI_COMM_WORLD);
            }



        } while (sign == 1);

        gettimeofday(&t_end, NULL);
        cout << "super time cost: "
            << 1000 * (t_end.tv_sec - t_start.tv_sec) +
            0.001 * (t_end.tv_usec - t_start.tv_usec) << "ms" << endl;
    }

    else
    {
        int sign;

        do
        {
            for (int i = rank; i < pasNum; i += num_proc)
            {
                MPI_Recv(&Pas[i], Num + 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            super(rank, num_proc);
            for (int i = rank; i < pasNum; i += num_proc)
            {
                MPI_Send(&Pas[i], Num + 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
            }

            MPI_Recv(&sign, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        } while (sign == 1);
    }
}
int main()
{
    init_A();
    init_P();
    f_ordinary1();

    init_A();
    init_P();
    f_ordinary();

    init_A();
    init_P();
    MPI_Init(NULL, NULL);

    f_mpi();

    MPI_Finalize();

}