#include "stdlib.h"
#include "matrix_lib.h"
#include "system_utility.h"

void showmat(Mat* A){
	if(A->row>0&&A->col>0){
		int k=0;
		printf("[");
		for(int i=1;i<=A->row;i++){
			for (int j=1;j<=A->col;j++){
				if(j<A->col){
					printf("%f\t",A->entries[k++]);
				}else{
					printf("%f",A->entries[k++]);
				}
			}
			if(i<A->row){
				printf("\n");
			}else{
				printf("]\n");
			}
		}
		printf("\n");
	}else{
		printf("[]");
	}
}

Mat* newmat(int r,int c,float d){
	Mat* M=(Mat*)malloc(sizeof(Mat));
	M->row=r;M->col=c;
	M->entries=(float*)malloc(sizeof(float)*r*c);
	int k=0;
	for(int i=1;i<=M->row;i++){
		for(int j=1;j<=M->col;j++){
			M->entries[k++]=d;
		}
	}
	return M;
}

void freemat(Mat* A){
	free(A->entries);
	free(A);
}

Mat* eye(int n){
	Mat* I=newmat(n,n,0);
	for(int i=1;i<=n;i++){
		I->entries[(i-1)*n+i-1]=1;
	}
	return I;
}

Mat* zeros(int r,int c){
	Mat* Z=newmat(r,c,0);
	return Z;
}

Mat* ones(int r,int c){
	Mat* O=newmat(r,c,1);
	return O;
}

Mat* randm(int r,int c,float l,float u){
	Mat* R=newmat(r,c,1);
	int k=0;
	for(int i=1;i<=r;i++){
		for(int j=1;j<=c;j++){
			float r=((float)rand())/((float)RAND_MAX);
			R->entries[k++]=l+(u-l)*r;
		}
	}
	return R;
}

float get(Mat* M,int r,int c){
	float d=M->entries[(r-1)*M->col+c-1];
	return d;
}

void set(Mat* M,int r,int c,float d){
	M->entries[(r-1)*M->col+c-1]=d;
}

Mat* scalermultiply(Mat* M,float c){
	Mat* B=newmat(M->row,M->col,0);
	int k=0;
	for(int i=0;i<M->row;i++){
		for(int j=0;j<M->col;j++){
			B->entries[k]=M->entries[k]*c;
			k+=1;
		}
	}
	return B;
}

Mat* sum(Mat* A,Mat* B){
	int r=A->row;
	int c=A->col;
	Mat* C=newmat(r,c,0);
	int k=0;
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			C->entries[k]=A->entries[k]+B->entries[k];
			k+=1;
		}
	}
	return C;
}

Mat* minus(Mat* A,Mat* B){
	int r=A->row;
	int c=A->col;
	Mat* C=newmat(r,c,0);
	int k=0;
	for(int i=0;i<r;i++){
		for(int j=0;j<c;j++){
			C->entries[k]=A->entries[k]-B->entries[k];
			k+=1;
		}
	}
	return C;
}

Mat* submat(Mat* A,int r1,int r2,int c1,int c2){
	Mat* B=newmat(r2-r1+1,c2-c1+1,0);
	int k=0;
	for(int i=r1;i<=r2;i++){
		for(int j=c1;j<=c2;j++){
			B->entries[k++]=A->entries[(i-1)*A->col+j-1];
		}
	}
	return B;
}

void submat2(Mat* A,Mat* B,int r1,int r2,int c1,int c2){
	int k=0;
	for(int i=r1;i<=r2;i++){
		for(int j=c1;j<=c2;j++){
			B->entries[k++]=A->entries[(i-1)*A->col+j-1];
		}
	}
}

Mat* multiply(Mat* A,Mat* B){
	int r1=A->row;
	int r2=B->row;
	int c1=A->col;
	int c2=B->col;
	if (r1==1&&c1==1){
		Mat* C=scalermultiply(B,A->entries[0]);
		return C;
	}else if (r2==1&&c2==1){
		Mat* C=scalermultiply(A,B->entries[0]);
		return C;
	}
	Mat* C=newmat(r1,c2,0);
	for(int i=1;i<=r1;i++){
		for(int j=1;j<=c2;j++){
			float de=0;
			for(int k=1;k<=r2;k++){
				de+=A->entries[(i-1)*A->col+k-1]*B->entries[(k-1)*B->col+j-1];
			}
			C->entries[(i-1)*C->col+j-1]=de;
		}
	}
	return C;
}

Mat* removerow(Mat* A,int r){
	Mat* B=newmat(A->row-1,A->col,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(i!=r){
				B->entries[k]=A->entries[(i-1)*A->col+j-1];
				k+=1;
			}
		}
	}
	return B;
}

Mat* removecol(Mat* A,int c){
	Mat* B=newmat(A->row,A->col-1,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(j!=c){
				B->entries[k]=A->entries[(i-1)*A->col+j-1];
				k+=1;
			}
		}
	}
	return B;
}

void removerow2(Mat* A,Mat* B,int r){
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(i!=r){
				B->entries[k++]=A->entries[(i-1)*A->col+j-1];
			}
		}
	}
}

void removecol2(Mat* A,Mat* B,int c){
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			if(j!=c){
				B->entries[k++]=A->entries[(i-1)*A->col+j-1];
			}
		}
	}
}

Mat* transpose(Mat* A){
	Mat* B=newmat(A->col,A->row,0);
	int k=0;
	for(int i=1;i<=A->col;i++){
		for(int j=1;j<=A->row;j++){
			B->entries[k]=A->entries[(j-1)*A->row+i-1];
			k+=1;
		}
	}
	return B;
}

float det(Mat* M){
	int r=M->row;
	int c=M->col;
	if(r==1&&c==1){
		float d=M->entries[0];
		return d;
	}
	Mat* M1=removerow(M,1);
	Mat* M2=newmat(M->row-1,M->col-1,0);
	float d=0, si=+1;
	for(int j=1;j<=M->col;j++){
		float c=M->entries[j-1];
		removecol2(M1,M2,j);
		d+=si*det(M2)*c;
		si*=-1;
	}
	freemat(M1);
	freemat(M2);
	return d;
}

float trace(Mat* A){
	float d=0;
	for(int i=1;i<=A->row;i++){
		d+=A->entries[(i-1)*A->row+i-1];
	}
	return d;
}

Mat* adjoint(Mat* A){
	Mat* B=newmat(A->row,A->col,0);
	Mat* A1=newmat(A->row-1,A->col,0);
	Mat* A2=newmat(A->row-1,A->col-1,0);
	for(int i=1;i<=A->row;i++){
		removerow2(A,A1,i);
		for(int j=1;j<=A->col;j++){
			removecol2(A1,A2,j);
			float si= 1/((float)(i+j));
			B->entries[(i-1)*B->col+j-1]=det(A2)*si;
		}
	}
	Mat* C=transpose(B);
	freemat(A1);
	freemat(A2);
	freemat(B);
	return C;
}

Mat* inverse(Mat* A){
	Mat* B=adjoint(A);
	float de=det(A);
	Mat* C=scalermultiply(B,1/de);
	freemat(B);
	return C;
}

Mat* copyvalue(Mat* A){
	Mat* B=newmat(A->row,A->col,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			B->entries[k]=A->entries[k];
			k++;
		}
	}
	return B;
}
Mat* triinverse(Mat* A){
	Mat* B=newmat(A->row,A->col,0);
	for(int i=1;i<=B->row;i++){
		for(int j=i;j<=B->col;j++){
			if(i==j){
				B->entries[(i-1)*B->col+j-1]=1/A->entries[(i-1)*A->col+j-1];
			}else{
				B->entries[(i-1)*B->col+j-1]=-A->entries[(i-1)*A->col+j-1]/A->entries[(j-1)*A->col+j-1];
			}
		}
	}
	return B;
}
Mat* rowechelon(Mat* A){
	if(A->row==1){
		for(int j=1;j<=A->col;j++){
			if(A->entries[j-1]!=0){
				Mat* B=scalermultiply(A,1/A->entries[j-1]);
				return B;
			}
		}
		Mat* B=newmat(1,A->col,0);
		return B;
	}
	Mat* B=copyvalue(A);
	int ind1=B->col+1;
	int ind2=1;
	for(int i=1;i<=B->row;i++){
		for(int j=1;j<=B->col;j++){
			if(B->entries[(i-1)*B->col+j-1]!=0&&j<ind1){
				ind1=j;
				ind2=i;
				break;
			}
		}
	}
	if(ind2>1){
		for(int j=1;j<=B->col;j++){
			float temp=B->entries[j-1];
			B->entries[j-1]=B->entries[(ind2-1)*B->col+j-1];
			B->entries[(ind2-1)*B->col+j-1]=temp;
		}
	}
	if(B->entries[0]!=0){
		float coeff=B->entries[0];
		for(int j=1;j<=B->col;j++){
			B->entries[j-1]/=coeff;
		}
		for(int i=2;i<=B->row;i++){
			coeff=B->entries[(i-1)*B->col];
			for(int j=1;j<=B->col;j++){
				B->entries[(i-1)*B->col+j-1]-=coeff*B->entries[j-1];
			}
		}
	}else{
		float coeff=0;
		for(int j=1;j<=B->col;j++){
			if(B->entries[j-1]!=0&&coeff==0){
				coeff=B->entries[j-1];
				B->entries[j-1]=1;
			}else if (B->entries[j-1]!=0){
				B->entries[j-1]/=coeff;
			}
		}
	}
	Mat* B1=removerow(B,1);
	Mat* B2=removecol(B1,1);
	Mat* Be=rowechelon(B2);
	for(int i=1;i<=Be->row;i++){
		for(int j=1;j<=Be->col;j++){
			B->entries[i*B->col+j]=Be->entries[(i-1)*Be->col+j-1];
		}
	}
	freemat(B1);
	freemat(B2);
	freemat(Be);
	return B;
}
Mat* hconcat(Mat* A,Mat* B){
	Mat* C=newmat(A->row,A->col+B->col,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			C->entries[k]=A->entries[(i-1)*A->col+j-1];
			k++;
		}
		for(int j=1;j<=B->col;j++){
			C->entries[k]=B->entries[(i-1)*B->col+j-1];
			k++;
		}
	}
	return C;
}
Mat* vconcat(Mat* A,Mat* B){
	Mat* C=newmat(A->row+B->row,A->col,0);
	int k=0;
	for(int i=1;i<=A->row;i++){
		for(int j=1;j<=A->col;j++){
			C->entries[k]=A->entries[(i-1)*A->col+j-1];
			k++;
		}
	}
	for(int i=1;i<=B->row;i++){
		for(int j=1;j<=B->col;j++){
			C->entries[k]=B->entries[(i-1)*B->col+j-1];
			k++;
		}
	}
	return C;
}

float innermultiply(Mat* a,Mat* b){
	float d=0;
	int n=a->row;
	if(a->col>n){
		n=a->col;
	}
	for(int i=1;i<=n;i++){
		d+=a->entries[i-1]*b->entries[i-1];
	}
	return d;
}
