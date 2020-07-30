#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define CRIT1 "1.txt"
#define CRIT2 "2.txt"
#define CRIT3 "3.txt"
#define RES "res_"
#define RT "rt_"
#define TRACE "my-trace"

void createReservation(int critLevel,int core,int hyper_per,int sTime,int period,int wcet)
{
    static char const res[]="resctl -n ";
  static char const res2[]=" -c 1 -t table-driven -m ";
  static char const i1[]="'";
  static char const i2[]="[";
  static char const i3[]=")";
  static char const i4[]=",";
  static char const i5[]=" ";
	
	int interval[50];
	char resctl[100];
	char append[10];
	char app2[10];
	sprintf(app2,"%d",hyper_per);
	strcpy(resctl,res);
	sprintf(append,"%d",core);
	strcat(resctl,append);
	strcat(resctl,res2);
	strcat(resctl,app2);
	int k=1;
	for(int j=0;j<hyper_per/period;j++)
	  {
	    interval[j+k-1]=j*period+sTime;
	    interval[j+k]=j*period+sTime+wcet;
	    k++;
	    }
        for(int i=0;i<hyper_per/period *2;i++)
	{
	
		char app3[10];
		if(i%2==0)
		{
			strcat(resctl,i5);
			strcat(resctl,i1);
			strcat(resctl,i2);
			sprintf(app3,"%d",interval[i]);
			strcat(resctl,app3);
			strcat(resctl,i4);
		}else
		{
			sprintf(app3,"%d",interval[i]);
			strcat(resctl,app3);
			strcat(resctl,i3);
			strcat(resctl,i1);
		}
	}
 	strcat(resctl,"\n");
	size_t twrite=sizeof resctl;
	//printf("%s\n",resctl);
	char filename[100];
	char convert[10];
	sprintf(convert,"%d",critLevel);
	strcpy(filename,RES);
	strcat(filename,convert);
	strcat(filename,".sh");
	FILE * f=fopen(filename,"a");
      	fprintf(f,resctl);
    fclose(f);
	
      
  

}
void createRTSPIN(int critLevel,int core,int period,int wcet)
{
 	static const char rt[]="rtspin -w -p 1 -r ";
	static const char end[]=" 1 & \n";
	static const char i1[]=" ";
	char app[10],app2[10],app3[10],rtspin[100];
	strcpy(rtspin,rt);
	sprintf(app,"%d",core);
	sprintf(app2,"%d",period);
	sprintf(app3,"%d",wcet);
	strcat(rtspin,app);
	strcat(rtspin,i1);
	strcat(rtspin,app3);
	strcat(rtspin,i1);
	strcat(rtspin,app2);
	strcat(rtspin,end);
	char rtfile[100];
	char convert[10];
	sprintf(convert,"%d",critLevel);
	strcpy(rtfile,RT);
	strcat(rtfile,convert);
	strcat(rtfile,".sh");
	FILE * f=fopen(rtfile,"a");
      	fprintf(f,rtspin);
    fclose(f);
	
}
  void executeRes()
  {
    execl("./reservation.sh", "reservation.sh", (char *)0);
    perror("coudn't execute script");
    exit(4);
  }
int main(int argc,char *argv[])
{
	static char const cres_setsched[]= "#!/bin/sh \n\n setsched Linux \n setsched P-RES \n\n\n";
	static char const st_trace[]="st-trace-schedule ";
	static char const rt_set[]="#!/bin/sh \n\n";
	size_t twrite=sizeof cres_setsched ;
	size_t rwrite=sizeof rt_set ;


	int filecnt=0;
	char cwd[256];
  	if(getcwd(cwd,sizeof(cwd))!=NULL)
	{
	DIR *dir;
	struct dirent *entry;
	if((dir=opendir(cwd))==NULL)

		{
			perror("Couldn't open dir");
			exit(10);
		}
	while((entry=readdir(dir))!=NULL)
	{
	 	if((entry->d_type==DT_REG) && (strcmp(entry->d_name,CRIT1)==0)|| (strcmp(entry->d_name,CRIT2)==0)|| (strcmp(entry->d_name,CRIT3)==0))
		{
			filecnt++; 
		}
		
	}
	closedir(dir);
	}

	int critLevel;
	for(critLevel=1;critLevel<=filecnt;critLevel++)
	{

		char resfile[100],rtfile[100],inputfile[100],convert[10];
		strcpy(resfile,RES);
		sprintf(convert,"%d",critLevel);
		strcat(resfile,convert);
		strcat(resfile,".sh");
		
		strcpy(rtfile,RT);
		strcat(rtfile,convert);
		strcat(rtfile,".sh");

		strcpy(inputfile,convert);
		strcat(inputfile,".txt");

    
    FILE * f=fopen(resfile,"w+");
      fwrite(cres_setsched,1,twrite-1,f);	
    fclose(f);
	
    FILE * fp=fopen(rtfile,"w+");
      fwrite(rt_set,1,rwrite-1,fp);
    fclose(fp);

	
	FILE *FP=fopen(inputfile,"r");
		char buff[100],ch;
		int howMany,hyper_per,i=0,arr[100],j=0;
	while(1)
	{
		ch=fgetc(FP);
		if(ch==EOF)
		{
			break;
		}
		else if(ch==' ' || ch=='\n')
		 {
		     arr[j]=atoi(buff);
			j++;
			bzero(buff,100);
			i=0;
			continue;
		}
		else
		{
		    buff[i]=ch;
			i++;
		}
	}
	  howMany=arr[0];
	  hyper_per=arr[1];
		int wcet[howMany],period[howMany],sTime[howMany],core[howMany],k=0;
	  for(i=2;i<j;i+=4)
		{
			
		   core[k]=arr[i];
		   k++;
		}
	  k=0;
	  for(i=3;i<j;i+=4)
		{
                    period[k]=arr[i];
		    k++;
		}
	  k=0;
	  for(i=5;i<j;i+=4)
		{
		    sTime[k]=arr[i];
		    k++;
		}
	 
	  k=0;
	  for(i=4;i<j;i+=4)
		{
		    wcet[k]=arr[i];
	            k++;
		}
	  k=0;

	fclose(FP);
	
	for(int i=0;i<howMany;i++)
	{
  	 createReservation(critLevel,core[i],hyper_per,sTime[i],period[i],wcet[i]);
	 createRTSPIN(critLevel,core[i],period[i],wcet[i]);
	}
		const char s[]="1";
		char c[2];
		strcpy(c,argv[critLevel]);
		if(strcmp(c,s)==0)
		 {
			FILE * ft=fopen(resfile,"a");
			char setsched[100];
			strcpy(setsched,st_trace);
			strcat(setsched,TRACE);
			strcat(setsched,"\n\n");
			size_t swrite=sizeof setsched ;
			fprintf(f,setsched);
			
		 }
		
	}
  // executeRes();
  return 0;
}
