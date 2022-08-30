#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>

void inputvalues(int bt[],int pr[],int n){
    for(int i=0;i<n;i++){
        printf("Enter Burst Time of PID-%d: ",i+1);//user bts
        scanf("%d",&bt[i]);
        printf("Enter Priority of PID-%d: ",i+1);//user priorities
        scanf("%d",&pr[i]);
    }
}

void sortarrays(int bt[],int pr[],int pid[],int n){
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - 1; j++)
            if (bt[j] > bt[j - 1])
            {
                int t = bt[j];
                bt[j] = bt[j + 1];
                bt[j + 1] = t;
                t = pr[j];
                pr[j] = pr[j + 1];
                pr[j + 1] = t;
                t = pid[j];
                pid[j] = pid[j + 1];
                pid[j + 1] = t;
            }
}
//priority based on order after sorting
void modify_priority(int pr[],int n){
    for(int i=0;i<n;i++){
        pr[i]+=i;
    }
}
//sort based on the newly assigned priority
void priosort(int bt[],int pr[],int pid[],int n){
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - 1; j++)
            if (pr[j] > pr[j + 1])
            {
                int t = bt[j];
                bt[j] = bt[j + 1];
                bt[j + 1] = t;
                t = pr[j];
                pr[j] = pr[j + 1];
                pr[j + 1] = t;
                t = pid[j];
                pid[j] = pid[j + 1];
                pid[j + 1] = t;
            }
}

int main(){
    int n /*no.of processes*/;
    printf("Enter no.of processes:");
    scanf("%d",&n);
    int ts; /*initial time quantum*/
    printf("Enter time slice:");
    scanf("%d",&ts);
    
    int bt[n] /*burst time*/, pr[n]/*priority*/, s[n]/*bs*/, wt[n]/*waiting time*/, tat[n]/*turnaround time*/;
    int its[n]/*obtained time quantum*/, dtq[n][n] /*time quantum*/, rbt[n]/*remaining burst time*/, pid[n]/*process id*/;
    for(int i=0;i<n;i++){
        pid[i]=i+1;
    }
    
    //input values such as burst time,priority for each process
    inputvalues(bt,pr,n);
    for (int i = 0; i < n; i++)
        rbt[i] = bt[i];
    printf("PID\tBurst time\tPriority");
    for(int i=0;i<n;i++){
        printf("\n%d\t\t%d\t\t%d",pid[i],bt[i],pr[i]);
    }
    printf("\n\n");
    //sorting the processes for assigning priority
    printf("Sorting based on Burst Time:\n");
    sortarrays(bt,pr,pid,n);
    printf("PID\tBurst time\tPriority");
    for(int i=0;i<n;i++){
        printf("\n%d\t\t%d\t\t%d",pid[i],bt[i],pr[i]);
    }
    printf("\n");
    //assigning priority
    for(int i=0;i<n;i++){
        tat[i]=0;
        wt[i]=0;
    }
    modify_priority(pr,n);
    priosort(bt,pr,pid,n);
    //main algorithm
    
    int flag=0,j=0;
    while (!flag)
    {
        for (int i = 0; i < n; i++)
        {
            //assigning priority component
            if (pr[i] > 0.5 * n)
                pr[i] = 0; //Not very imp
            else if (pr[i] > 0.2 * n)
                pr[i] = 1; //imp
            else
                pr[i] = 2; //very imp

            //assigning shortness component
            if (i != 0)
                if ((bt[i] - bt[i - 1]) > 0)
                    s[i] = 0; //Long process
            its[i] = ts + bt[i] + s[i] + pr[i];

            //allocating tq for first round
            if (j == 0)
            {
                if (s[i] == 1)
                    dtq[j][i] = its[i];
                else
                    dtq[j][i] = round(0.5 * (float)its[i]);
                if (rbt[i] < dtq[j][i])
                    dtq[j][i] = rbt[i];
                rbt[i] = rbt[i] - dtq[j][i];
            }
            else

            //allocating tq for further rounds
            {
                if (rbt[i] <= 0)
                    dtq[j][i] = 0;
                else if (s[i] == 1)
                    dtq[j][i] = 2 * dtq[j - 1][i];
                else
                    dtq[j][i] = 1.5 * dtq[j - 1][i];
                if (rbt[i] < dtq[j][i])
                    dtq[j][i] = rbt[i];
                rbt[i] = rbt[i] - dtq[j][i];
            }
        }
        j++;
        //j has no.of rounds.
        flag = -1;
        for (int i = 0; i < n; i++)
            if (rbt[i] > 0)
                flag = 0;
    }
   
    printf("\n\nDynamic Time Quantum:\n");
    for (int x = 0; x < j; x++)
    {
        printf("Round %d: ", x+1);
        for (int y = 0; y < n; y++)
            printf("%d ", dtq[x][y]);
            printf("\n");
    }
    //calculating tat,wt
    for (int x = 0; x < n; x++)
    {
        flag = -1;
        for (int y = 0; y < j; y++)
        {
            for (int z = 0; z < n; z++)
            {
                if (z != x)
                    wt[x] += dtq[y][z];
                else if (z == x && dtq[y + 1][z] == 0)
                {
                    flag = 0;
                    break;
                }
            }
            tat[x] += dtq[y][x];
            if (flag == 0)
                break;
        }
        tat[x] += wt[x];
    }

    //calculating average tat,wt
    float avgwt = 0, avgtat = 0;
    for (int i = 0; i < n; i++)
    {
        avgwt += wt[i];
        avgtat += tat[i];
    }
    avgwt /= n;
    avgtat /= n;

    //printing output
    printf("PID\tBT\tPr\tTAT\tWT");
    for(int i=0;i<n;i++){
        printf("\n%d\t%d\t%d\t%d\t%d",pid[i],bt[i],pr[i],tat[i],wt[i]);
    }
    printf("\n\nAverage waiting time: %f\n",avgwt);
    printf("\nAverage turnaround time: %f\n",avgtat);;
    return 0;
}