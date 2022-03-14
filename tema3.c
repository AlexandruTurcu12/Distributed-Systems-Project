#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int numtasks, rank, dest, count, tag;
    int elem;
    MPI_Status status;
    int err = atoi(argv[2]);

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0){

        int dim_local, dim1, dim2;
        FILE *file = fopen("cluster0.txt", "rt");
        fscanf(file, "%d", &dim_local);
        dim_local++;
        int *v0 = malloc(dim_local * sizeof(int));
        v0[0] = 0;
        for(int i = 1; i < dim_local; i++){
            fscanf(file, "%d", &elem);
            v0[i] = elem;
        }
        fclose(file);

        for(int i = 1; i < dim_local; i++){
            MPI_Send(&rank, 1, MPI_INT, v0[i], v0[i], MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,v0[i]);
        }

        MPI_Send(&dim_local, 1, MPI_INT, 1, 100, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",0,1);
        MPI_Send(&dim_local, 1, MPI_INT, 2, 101, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",0,2);
        MPI_Recv(&dim1, 1, MPI_INT, 1, 102, MPI_COMM_WORLD, &status);
        MPI_Recv(&dim2, 1, MPI_INT, 2, 104, MPI_COMM_WORLD, &status);

        int dim_final = dim_local + dim1 + dim2;
        for(int i = 1; i < dim_local; i++){
            MPI_Send(&dim_final, 1, MPI_INT, v0[i], v0[i], MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,v0[i]);
        }

        int *v_final = malloc(dim_final * sizeof(int));
        int *v1 = malloc(dim1 * sizeof(int));
        int *v2 = malloc(dim2 * sizeof(int));

        MPI_Send(v0, dim_local, MPI_INT, 1, 100, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",0,1);
        MPI_Send(v0, dim_local, MPI_INT, 2, 101, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",0,2);
        MPI_Recv(v1, dim1, MPI_INT, 1, 102, MPI_COMM_WORLD, &status);
        MPI_Recv(v2, dim2, MPI_INT, 2, 104, MPI_COMM_WORLD, &status);

        for(int i = 0; i < dim_local; i++){
            v_final[i] = v0[i];
        }
        for(int i = 0; i < dim1; i++){
            v_final[i + dim_local] = v1[i];
        }
        for(int i = 0; i < dim2; i++){
            v_final[i + dim_local + dim1] = v2[i];
        }
        printf("%d -> ",rank);
        for(int i = 0; i < dim_final; i++){
            if(v_final[i] == 0 || v_final[i] == 1 || v_final[i] == 2)
                printf("%d:",v_final[i]);
            else if(i == dim_final - 1)
                printf("%d\n",v_final[i]);
            else if(i == dim_local - 1 || i == dim_local + dim1 - 1)
                printf("%d ",v_final[i]);
            else
                printf("%d,",v_final[i]);
        }

        for(int i = 1; i < dim_local; i++){
            MPI_Send(&dim_local, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,v0[i]);
            MPI_Send(&dim1, 1, MPI_INT, v0[i], 1, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,v0[i]);
            MPI_Send(&dim2, 1, MPI_INT, v0[i], 2, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,v0[i]);
            MPI_Send(v_final, dim_final, MPI_INT, v0[i], v0[i], MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,v0[i]);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        int N = atoi(argv[1]);
        int offset = 0;
        int *v_calc = malloc(N * sizeof(int));
        for(int i = 0; i < N; i++){
            v_calc[i] = i;
        }

        int *v_calc0 = malloc(N / (numtasks - 3) * (dim_local - 1) * sizeof(int));
        for(int i = 0; i < N / (numtasks - 3) * (dim_local - 1); i++){
            v_calc0[i] = v_calc[i];
            offset++;
        }   

        int *v_calc2 = malloc(N / (numtasks - 3) * (dim2 - 1) * sizeof(int));
        for(int i = 0; i < N / (numtasks - 3) * (dim2 - 1); i++){
            v_calc2[i] = v_calc[offset];
            offset++;
        }

        int *v_calc1 = malloc((N / (numtasks - 3) * (dim1 - 1) + N % (numtasks - 3)) * sizeof(int));
        for(int i = 0; i < N / (numtasks - 3) * (dim1 - 1) + N % (numtasks - 3); i++){
            v_calc1[i] = v_calc[offset];
            offset++;
        }

        offset = 0;
        int aux1 = N / (numtasks - 3) * (dim_local - 1);
        int aux2 = N / (numtasks - 3);
        int aux3 = N / (numtasks - 3) * (dim2 - 1);
        int aux4 = N / (numtasks - 3) * (dim1 - 1) + N % (numtasks - 3);
        int aux5 = N / (numtasks - 3) + N % (numtasks - 3);
        for(int i = 1; i < dim_local; i++){
            MPI_Send(&aux1, 1, MPI_INT, v0[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,v0[i]);
            MPI_Send(&aux2, 1, MPI_INT, v0[i], 1, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,v0[i]);
            MPI_Send(v_calc0, aux1, MPI_INT, v0[i], v0[i], MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,v0[i]);
            MPI_Send(&offset, 1, MPI_INT, v0[i], 100, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,v0[i]);
            MPI_Recv(v_calc0, aux1, MPI_INT, v0[i], v0[i], MPI_COMM_WORLD, &status);
            offset += N / (numtasks - 3);

        }

        for(int i = 0; i < aux1; i++){
            v_calc[i] = v_calc0[i];
        }

        MPI_Send(&aux3, 1, MPI_INT, 2, 100, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",0,2);
        MPI_Send(v_calc2, aux3, MPI_INT, 2, 101, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",0,2);
        MPI_Send(&aux2, 1, MPI_INT, 2, 102, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",0,2);

        int offset_aux;
        MPI_Recv(v_calc2, aux3, MPI_INT, 2, 200, MPI_COMM_WORLD, &status);
        MPI_Recv(&offset_aux, 1, MPI_INT, 2, 201, MPI_COMM_WORLD, &status);
        for(int i = 0; i < aux3; i++){
            v_calc[i + offset] = v_calc2[i];
        }
        offset += offset_aux;

        if (err == 0){
            MPI_Send(&aux4, 1, MPI_INT, 1, 100, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,1);
            MPI_Send(v_calc1, aux4, MPI_INT, 1, 101, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,1);
            MPI_Send(&aux2, 1, MPI_INT, 1, 102, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,1);
            MPI_Send(&aux5, 1, MPI_INT, 1, 103, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,1);

            MPI_Recv(v_calc1, aux4, MPI_INT, 1, 300, MPI_COMM_WORLD, &status);
            MPI_Recv(&offset_aux, 1, MPI_INT, 1, 301, MPI_COMM_WORLD, &status);
            for(int i = 0; i < aux4; i++){
                v_calc[i + offset] = v_calc1[i];
            }
        } else if (err == 1){
            MPI_Send(&aux4, 1, MPI_INT, 2, 400, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,2);
            MPI_Send(v_calc1, aux4, MPI_INT, 2, 401, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,2);
            MPI_Send(&aux2, 1, MPI_INT, 2, 402, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,2);
            MPI_Send(&aux5, 1, MPI_INT, 2, 403, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",0,2);

            MPI_Recv(v_calc1, aux4, MPI_INT, 2, 300, MPI_COMM_WORLD, &status);
            MPI_Recv(&offset_aux, 1, MPI_INT, 2, 301, MPI_COMM_WORLD, &status);
            for(int i = 0; i < aux4; i++){
                v_calc[i + offset] = v_calc1[i];
            }
        }

        printf("Rezultat: ");
        for(int i = 0; i < N; i++){
            printf("%d ",v_calc[i]);
        }
        printf("\n");

    }

    if (rank == 1){

        int dim_local, dim0, dim2;
        FILE *file = fopen("cluster1.txt", "rt");
        fscanf(file, "%d", &dim_local);
        dim_local++;
        int *v1 = malloc(dim_local * sizeof(int));
        v1[0] = 1;
        for(int i = 1; i < dim_local; i++){
            fscanf(file, "%d", &elem);
            v1[i] = elem;
        }
        fclose(file);

        for(int i = 1; i < dim_local; i++){
            MPI_Send(&rank, 1, MPI_INT, v1[i], v1[i], MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,v1[i]);
        }

        MPI_Send(&dim_local, 1, MPI_INT, 0, 102, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",1,0);
        MPI_Send(&dim_local, 1, MPI_INT, 2, 103, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",1,2);
        MPI_Recv(&dim0, 1, MPI_INT, 0, 100, MPI_COMM_WORLD, &status);
        MPI_Recv(&dim2, 1, MPI_INT, 2, 105, MPI_COMM_WORLD, &status);

        int dim_final = dim0 + dim_local + dim2;
        for(int i = 1; i < dim_local; i++){
            MPI_Send(&dim_final, 1, MPI_INT, v1[i], v1[i], MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,v1[i]);
        }

        int *v_final = malloc(dim_final * sizeof(int));
        int *v0 = malloc(dim0 * sizeof(int));
        int *v2 = malloc(dim2 * sizeof(int));

        MPI_Send(v1, dim_local, MPI_INT, 0, 102, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",1,0);
        MPI_Send(v1, dim_local, MPI_INT, 2, 103, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",1,2);
        MPI_Recv(v0, dim0, MPI_INT, 0, 100, MPI_COMM_WORLD, &status);
        MPI_Recv(v2, dim2, MPI_INT, 2, 105, MPI_COMM_WORLD, &status);

        for(int i = 0; i < dim0; i++){
            v_final[i] = v0[i];
        }
        for(int i = 0; i < dim_local; i++){
            v_final[i + dim0] = v1[i];
        }
        for(int i = 0; i < dim2; i++){
            v_final[i + dim0 + dim_local] = v2[i];
        }
        printf("%d -> ",rank);
        for(int i = 0; i < dim_final; i++){
            if(v_final[i] == 0 || v_final[i] == 1 || v_final[i] == 2)
                printf("%d:",v_final[i]);
            else if(i == dim_final - 1)
                printf("%d\n",v_final[i]);
            else if(i == dim0 - 1 || i == dim0 + dim_local - 1)
                printf("%d ",v_final[i]);
            else
                printf("%d,",v_final[i]);
        }

        for(int i = 1; i < dim_local; i++){
            MPI_Send(&dim0, 1, MPI_INT, v1[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,v1[i]);
            MPI_Send(&dim_local, 1, MPI_INT, v1[i], 1, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,v1[i]);
            MPI_Send(&dim2, 1, MPI_INT, v1[i], 2, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,v1[i]);
            MPI_Send(v_final, dim_final, MPI_INT, v1[i], v1[i], MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,v1[i]);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        int aux2,aux4,aux5,offset;
        if (err == 0){
            MPI_Recv(&aux4, 1, MPI_INT, 0, 100, MPI_COMM_WORLD, &status);
        } else if (err == 1){
            MPI_Recv(&aux4, 1, MPI_INT, 2, 400, MPI_COMM_WORLD, &status);
        }
        int *v_calc1 = malloc(aux4 * sizeof(int));
        if (err == 0){
            MPI_Recv(v_calc1, aux4, MPI_INT, 0, 101, MPI_COMM_WORLD, &status);
            MPI_Recv(&aux2, 1, MPI_INT, 0, 102, MPI_COMM_WORLD, &status);
            MPI_Recv(&aux5, 1, MPI_INT, 0, 103, MPI_COMM_WORLD, &status);
        } else if (err == 1){
            MPI_Recv(v_calc1, aux4, MPI_INT, 2, 401, MPI_COMM_WORLD, &status);
            MPI_Recv(&aux2, 1, MPI_INT, 2, 402, MPI_COMM_WORLD, &status);
            MPI_Recv(&aux5, 1, MPI_INT, 2, 403, MPI_COMM_WORLD, &status);
        }

        int offset_aux = 0;
        for(int i = 1; i < dim_local - 1; i++){
            MPI_Send(&aux4, 1, MPI_INT, v1[i], 1, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,v1[i]);
            MPI_Send(&aux2, 1, MPI_INT, v1[i], 2, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,v1[i]);
            MPI_Send(v_calc1, aux4, MPI_INT, v1[i], v1[i], MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,v1[i]);
            MPI_Send(&offset_aux, 1, MPI_INT, v1[i], 100, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,v1[i]);
            MPI_Recv(v_calc1, aux4, MPI_INT, v1[i], v1[i], MPI_COMM_WORLD, &status);
            offset_aux += aux2;
        }

        MPI_Send(&aux4, 1, MPI_INT, v1[dim_local - 1], 1, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",1,v1[dim_local - 1]);
        MPI_Send(&aux5, 1, MPI_INT, v1[dim_local - 1], 2, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",1,v1[dim_local - 1]);
        MPI_Send(v_calc1, aux4, MPI_INT, v1[dim_local - 1], v1[dim_local - 1], MPI_COMM_WORLD);
        printf("M(%d,%d)\n",1,v1[dim_local - 1]);
        MPI_Send(&offset_aux, 1, MPI_INT, v1[dim_local - 1], 100, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",1,v1[dim_local - 1]);
        MPI_Recv(v_calc1, aux4, MPI_INT, v1[dim_local - 1], v1[dim_local - 1], MPI_COMM_WORLD, &status);
        offset_aux += aux2;

        if (err == 0){
            MPI_Send(v_calc1, aux4, MPI_INT, 0, 300, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,0);
            MPI_Send(&offset_aux, 1, MPI_INT, 0, 301, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,0);
        } else if (err == 1){
            MPI_Send(v_calc1, aux4, MPI_INT, 2, 300, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,2);
            MPI_Send(&offset_aux, 1, MPI_INT, 2, 301, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",1,2);
        }

    }

    if (rank == 2){
        
        int dim_local, dim0, dim1;
        FILE *file = fopen("cluster2.txt", "rt");
        fscanf(file, "%d", &dim_local);
        dim_local++;
        int *v2 = malloc(dim_local * sizeof(int));
        v2[0] = 2;
        for(int i = 1; i < dim_local; i++){
            fscanf(file, "%d", &elem);
            v2[i] = elem;
        }
        fclose(file);

        for(int i = 1; i < dim_local; i++){
            MPI_Send(&rank, 1, MPI_INT, v2[i], v2[i], MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,v2[i]);
        }

        MPI_Send(&dim_local, 1, MPI_INT, 0, 104, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",2,0);
        MPI_Send(&dim_local, 1, MPI_INT, 1, 105, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",2,1);
        MPI_Recv(&dim0, 1, MPI_INT, 0, 101, MPI_COMM_WORLD, &status);
        MPI_Recv(&dim1, 1, MPI_INT, 1, 103, MPI_COMM_WORLD, &status);

        int dim_final = dim0 + dim1 + dim_local;
        for(int i = 1; i < dim_local; i++){
            MPI_Send(&dim_final, 1, MPI_INT, v2[i], v2[i], MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,v2[i]);
        }

        int *v_final = malloc(dim_final * sizeof(int));
        int *v0 = malloc(dim0 * sizeof(int));
        int *v1 = malloc(dim1 * sizeof(int));

        MPI_Send(v2, dim_local, MPI_INT, 0, 104, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",2,0);
        MPI_Send(v2, dim_local, MPI_INT, 1, 105, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",2,1);
        MPI_Recv(v0, dim0, MPI_INT, 0, 101, MPI_COMM_WORLD, &status);
        MPI_Recv(v1, dim1, MPI_INT, 1, 103, MPI_COMM_WORLD, &status);

        for(int i = 0; i < dim0; i++){
            v_final[i] = v0[i];
        }
        for(int i = 0; i < dim1; i++){
            v_final[i + dim0] = v1[i];
        }
        for(int i = 0; i < dim_local; i++){
            v_final[i + dim0 + dim1] = v2[i];
        }
        printf("%d -> ",rank);
        for(int i = 0; i < dim_final; i++){
            if(v_final[i] == 0 || v_final[i] == 1 || v_final[i] == 2)
                printf("%d:",v_final[i]);
            else if(i == dim_final - 1)
                printf("%d\n",v_final[i]);
            else if(i == dim0 - 1 || i == dim0 + dim1 - 1)
                printf("%d ",v_final[i]);
            else
                printf("%d,",v_final[i]);
        }

        for(int i = 1; i < dim_local; i++){
            MPI_Send(&dim0, 1, MPI_INT, v2[i], 0, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,v2[i]);
            MPI_Send(&dim1, 1, MPI_INT, v2[i], 1, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,v2[i]);
            MPI_Send(&dim_local, 1, MPI_INT, v2[i], 2, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,v2[i]);
            MPI_Send(v_final, dim_final, MPI_INT, v2[i], v2[i], MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,v2[i]);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        int aux3,aux2,offset,aux4,aux5;
        MPI_Recv(&aux3, 1, MPI_INT, 0, 100, MPI_COMM_WORLD, &status);
        int *v_calc2 = malloc(aux3 * sizeof(int));
        MPI_Recv(v_calc2, aux3, MPI_INT, 0, 101, MPI_COMM_WORLD, &status);
        MPI_Recv(&aux2, 1, MPI_INT, 0, 102, MPI_COMM_WORLD, &status);

        int offset_aux = 0;

        for(int i = 1; i < dim_local; i++){
            MPI_Send(&aux3, 1, MPI_INT, v2[i], 2, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,v2[i]);
            MPI_Send(&aux2, 1, MPI_INT, v2[i], 3, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,v2[i]);
            MPI_Send(v_calc2, aux3, MPI_INT, v2[i], v2[i], MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,v2[i]);
            MPI_Send(&offset_aux, 1, MPI_INT, v2[i], 100, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,v2[i]);
            MPI_Recv(v_calc2, aux3, MPI_INT, v2[i], v2[i], MPI_COMM_WORLD, &status);
            offset_aux += aux2;

        }

        MPI_Send(v_calc2, aux3, MPI_INT, 0, 200, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",2,0);
        MPI_Send(&offset_aux, 1, MPI_INT, 0, 201, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",2,0);

        if (err == 1){
            MPI_Recv(&aux4, 1, MPI_INT, 0, 400, MPI_COMM_WORLD, &status);
            int *v_calc1 = malloc(aux4 * sizeof(int));
            MPI_Recv(v_calc1, aux4, MPI_INT, 0, 401, MPI_COMM_WORLD, &status);
            MPI_Recv(&aux2, 1, MPI_INT, 0, 402, MPI_COMM_WORLD, &status);
            MPI_Recv(&aux5, 1, MPI_INT, 0, 403, MPI_COMM_WORLD, &status);

            MPI_Send(&aux4, 1, MPI_INT, 1, 400, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,1);
            MPI_Send(v_calc1, aux4, MPI_INT, 1, 401, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,1);
            MPI_Send(&aux2, 1, MPI_INT, 1, 402, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,1);
            MPI_Send(&aux5, 1, MPI_INT, 1, 403, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,1);

            MPI_Recv(v_calc1, aux4, MPI_INT, 1, 300, MPI_COMM_WORLD, &status);
            MPI_Recv(&offset_aux, 1, MPI_INT, 1, 301, MPI_COMM_WORLD, &status);

            MPI_Send(v_calc1, aux4, MPI_INT, 0, 300, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,0);
            MPI_Send(&offset_aux, 1, MPI_INT, 0, 301, MPI_COMM_WORLD);
            printf("M(%d,%d)\n",2,0);
            
        }

    }

    if(rank !=0 && rank !=1 && rank!=2){
        int coord, dim0, dim1, dim2, dim_final;
        MPI_Recv(&coord, 1, MPI_INT, MPI_ANY_SOURCE, rank, MPI_COMM_WORLD, &status);
        MPI_Recv(&dim_final, 1, MPI_INT, coord, rank, MPI_COMM_WORLD, &status);

        int *v_final = malloc(dim_final * sizeof(int));
        MPI_Recv(&dim0, 1, MPI_INT, coord, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&dim1, 1, MPI_INT, coord, 1, MPI_COMM_WORLD, &status);
        MPI_Recv(&dim2, 1, MPI_INT, coord, 2, MPI_COMM_WORLD, &status);
        MPI_Recv(v_final, dim_final, MPI_INT, coord, rank, MPI_COMM_WORLD, &status);

        printf("%d -> ",rank);
        for(int i = 0; i < dim_final; i++){
            if(v_final[i] == 0 || v_final[i] == 1 || v_final[i] == 2)
                printf("%d:",v_final[i]);
            else if(i == dim_final - 1)
                printf("%d\n",v_final[i]);
            else if(i == dim0 - 1 || i == dim0 + dim1 - 1)
                printf("%d ",v_final[i]);
            else
                printf("%d,",v_final[i]);
        }

        MPI_Barrier(MPI_COMM_WORLD);

        int nr_calc, nr_calc2, offset;
        MPI_Recv(&nr_calc, 1, MPI_INT, coord, coord, MPI_COMM_WORLD, &status);
        MPI_Recv(&nr_calc2, 1, MPI_INT, coord, coord+1, MPI_COMM_WORLD, &status);
        int *v_calc = malloc(nr_calc * sizeof(int));
        MPI_Recv(v_calc, nr_calc, MPI_INT, coord, rank, MPI_COMM_WORLD, &status);
        MPI_Recv(&offset, 1, MPI_INT, coord, 100, MPI_COMM_WORLD, &status);
        for(int i = 0; i < nr_calc2; i++){
            v_calc[i + offset] *= 2;
        }
        MPI_Send(v_calc, nr_calc, MPI_INT, coord, rank, MPI_COMM_WORLD);
        printf("M(%d,%d)\n",rank,coord);

    }

    MPI_Finalize();
}