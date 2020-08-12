#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define RES "res_"
#define RT "rt_"
#define TRACE "my-trace"
/*
	Functie care creeaza rezervari luand ca parametrii: corul,nivelul de criticalitate, id rezervarii,hyper-perioada,timpul de start, perioada si wcetul.
	Fiecare apelare a functiei creaza o rezervare pe care o salveaza intr-un fisier .sh denumit res_critLevel&core, spre exemplu res_01 */
void createReservation(int core,int critLevel,int id,int hyper_per,int sTime,int period,int wcet)
{
    static char const res[]="resctl -n ";
    static char const r[]=" -c ";
  static char const res2[]=" -t table-driven -m ";
  static char const i1[]="'";
  static char const i2[]="[";
  static char const i3[]=")";
  static char const i4[]=",";
  static char const i5[]=" ";
	
	int interval[50];
	char resctl[100];
	char append[10],r1[10];
	char app2[10];
	sprintf(app2,"%d",hyper_per);
	strcpy(resctl,res);
	sprintf(r1,"%d",core);
	sprintf(append,"%d",id);
	strcat(resctl,append);
	strcat(resctl,r);
	strcat(resctl,r1);
	strcat(resctl,res2);
	strcat(resctl,app2);
	int k=1;

	/* se calculeaza intervalurile rezervarii */

	for(int j=0;j<hyper_per/period;j++)
	  {
	    interval[j+k-1]=j*period+sTime;
	    interval[j+k]=j*period+sTime+wcet;
	    k++;
	    }
	
	/* se concateneaza intervalurile in parametrul "resctl" */

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

	/* se adauga in fisier parametrul resctl reprezentand rezervarea*/

 	strcat(resctl,"\n");
	size_t twrite=sizeof resctl;
	//printf("%s\n",resctl);
	char filename[100];
	char convert[10],convcore[10];
	sprintf(convert,"%d",critLevel);
	sprintf(convcore,"%d",core);
	strcpy(filename,RES);
	strcat(filename,convert);
	strcat(filename,convcore);
	strcat(filename,".sh");
	FILE * f=fopen(filename,"a");
      	fprintf(f,resctl);
    fclose(f);
	
      
  

}
	
		/* Functie care creaza task-uri.Primeste ca parametrii: core,nivelul de criticalitate, id-ul task-ului, perioada si wcet-ul*/

void createRTSPIN(int core,int critLevel,int id,int period,int wcet)
{
	/* se creaza in parametrul rtspin task-ul propriu-zis*/

 	static const char rt[]="rtspin -w -p ";
	static const char rt2[]=" -r ";
	static const char end[]=" 1 & \n";
	static const char i1[]=" ";
	char app[10],app2[10],app3[10],rtspin[100],c[10];
	strcpy(rtspin,rt);
	sprintf(app,"%d",id);
	sprintf(app2,"%d",period);
	sprintf(app3,"%d",wcet);
	sprintf(c,"%d",core);
	strcat(rtspin,c);
	strcat(rtspin,rt2);
	strcat(rtspin,app);
	strcat(rtspin,i1);
	strcat(rtspin,app3);
	strcat(rtspin,i1);
	strcat(rtspin,app2);
	strcat(rtspin,end);

	/* se adauga in fisier parametrul rtspin */

	char rtfile[100];
	char convert[10];
	sprintf(convert,"%d",critLevel);
	strcpy(rtfile,RT);
	strcat(rtfile,convert);
	strcat(rtfile,c);
	strcat(rtfile,".sh");
	FILE * f=fopen(rtfile,"a");
      	fprintf(f,rtspin);
    fclose(f);
	
}
 /* void executeRes(char filename[])
  {
    execl("filename", "chmod", "+x",  "filename", (char *)0);
    perror("coudn't execute script");
    exit(4);
  }
  void executeRt(char filename[],int wcet)
  {
	int msecTime=0;
	clock_t before=clock();
	while(msecTime<wcet)
	{
		clock_t actual=clock() -before;
		msecTime=difference*1000 / CLOCKS_PER_SEC;
		execl("filename", "chmod", "+x", "filename", (char*)0);
		perror("coudn't execute script");
		exit(99);
	}
	printf("%d", msecTime%1000);
  }*/
int main(int argc,char *argv[])
{
	static char const cres_setsched[]= "#!/bin/sh \n\n setsched Linux \n setsched P-RES \n\n\n";
	static char const st_trace[]="st-trace-schedule ";
	static char const rt_set[]="#!/bin/sh \n\n";
	size_t twrite=sizeof cres_setsched ;
	size_t rwrite=sizeof rt_set ;

	int filecnt=0;
	char cwd[256],files[100][256];

	/* se verifica in directorul curent numarul de fisiere care au denumirea "param" si se salveaza in "filecnt"*/

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
	 	if((entry->d_type==DT_REG) && (strstr(entry->d_name,"param")))
		{
			strcpy(files[filecnt],entry->d_name);
			filecnt++; 
		}
		
	}
	closedir(dir);
	}
	int noFile;
	for(noFile=0;noFile<filecnt;noFile++)
	{
		
		
		char resfile[100],rtfile[100],inputfile[100],convert[10];
		strcpy(inputfile,files[noFile]); //se ia fiecare fisier de intrare
		

	/* 
		Se deschide fisierul de intrare si se citeste continutul intr-un array
	*/

		char buff[100],ch,add[10],add1[10];
		int howMany,hyper_per,i=0,arr[100],j=0,core,critLevel;
	
	FILE *FP=fopen(inputfile,"r");
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
	fclose(FP);
	  howMany=arr[0];
	  core=arr[2];
	  critLevel=arr[1];
	  hyper_per=arr[3];
	  
	  sprintf(add,"%d",critLevel);
	  sprintf(add1,"%d",core);

	
	/* Pentru fiecare fisier se adauga in nume corul si nivelul de criticalitate.
		Se creeaza fisierul rezervarii si task-ului */

		strcpy(resfile,RES);
		strcat(resfile,add);
		strcat(resfile,add1);
		strcat(resfile,".sh");
		
		strcpy(rtfile,RT);
		strcat(rtfile,add);
		strcat(rtfile,add1);
		strcat(rtfile,".sh");


	 /* se deschid fisierele de rezervare si task pentru resetare schedule*/

	FILE * f=fopen(resfile,"w+");
        fwrite(cres_setsched,1,twrite-1,f);	
        fclose(f);
	
    	FILE * fp=fopen(rtfile,"w+");
        fwrite(rt_set,1,rwrite-1,fp);
    	fclose(fp);
		int wcet[howMany],period[howMany],sTime[howMany],id[howMany],k=0;

	/* se creeaza un array pentru fiecare parametru al rezervarii/task-ului */

	  for(i=4;i<j;i+=4)
		{
			
		   id[k]=arr[i];
		   k++;
		}
	  k=0;
	  for(i=5;i<j;i+=4)
		{
                    period[k]=arr[i];
		    k++;
		}
	  k=0;
	  for(i=7;i<j;i+=4)
		{
		    sTime[k]=arr[i];
		    k++;
		}
	 
	  k=0;
	  for(i=6;i<j;i+=4)
		{
		    wcet[k]=arr[i];
	            k++;
		}
	  k=0;


	/* pentru numarul de rezervari intr-un fiesier se apeleaza functiile */

	for(int i=0;i<howMany;i++)
	{
  	 createReservation(core,critLevel,id[i],hyper_per,sTime[i],period[i],wcet[i]);
	 createRTSPIN(core,critLevel,id[i],period[i],wcet[i]);
	}
		const char s[]="1";
		char c[2];
		strcpy(c,argv[noFile+1]);

		/* se ia din argumentele trimise si se verifica daca se adauga in fisier la final st_trace */

		if(strcmp(c,s)==0)
		 {
			FILE * ft=fopen(resfile,"a");
			char setsched[100];
			strcpy(setsched,st_trace);
			strcat(setsched,TRACE);
			strcat(setsched,"\n\n");
			size_t swrite=sizeof setsched ;
			fprintf(ft,setsched);
			
		 }

		/*int y=0;
		executeRes(resfile);
		if(wcet[y]!=NULL)
		executeRt(rtfile,wcet[y]);
		y++;*/
	}

  // executeRes();
  return 0;
}
