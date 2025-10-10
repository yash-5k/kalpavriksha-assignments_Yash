#include<stdio.h>
#include<string.h>
#include<ctype.h>
typedef struct StudentPerformace
{
    int rollno;
    char name[20];
    int subject1Marks;
    int subject2Marks;
    int subject3Marks;
} stp;
void getroll(stp arr[],int i,int n){
    if(i==n) return;
    printf("%d",arr[i].rollno);
    getroll(arr,i+1,n);
}
int main(){

    int testcases=0;
    scanf("%d",&testcases);

    stp s[testcases];
    for(int i=0;i<testcases;i++){
        scanf("%d %s %d %d %d",&s[i].rollno,s[i].name,&s[i].subject1Marks,&s[i].subject2Marks,&s[i].subject3Marks);
    }

    
    for(int i=0;i<testcases;i++){
        stp curstudent=s[i];
        int totalmarks=curstudent.subject1Marks+curstudent.subject2Marks+curstudent.subject3Marks;
        double average=totalmarks/3.0;

        if(average>=85){
            printf("Roll: %d\nName: %s\nTotal Marks: %d\nAverage: %.2f\nGrade:A\n",s[i].rollno,s[i].name,totalmarks,average);
            printf("Performance :*****");
        }else if(average>=70){
            printf("Roll: %d\nName: %s\nTotal Marks: %d\nAverage: %.2f\nGrade:B\n",s[i].rollno,s[i].name,totalmarks,average);
            printf("Performance :****");
        }else if(average>=50){
            printf("Roll: %d\nName: %s\nTotal Marks: %d\nAverage: %.2f\nGrade:C\n",s[i].rollno,s[i].name,totalmarks,average);
            printf("Performance :***");
        }else if(average>=35){
            printf("Roll: %d\nName: %s\nTotal Marks: %d\nAverage: %.2f\nGrade:D\n",s[i].rollno,s[i].name,totalmarks,average);
            printf("Performance :**");
        }else{
            printf("Roll: %d\nName: %s\nTotal Marks: %d\nAverage: %.2f\nGrade:F\n",s[i].rollno,s[i].name,totalmarks,average);
        }
        printf("\n");

    }

    printf("List of roll numbers (via recursion):");
    getroll(s,0,testcases);
    printf("\n");

    return 0;
}