/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"

#include "synchconsole.h"
#include "syscall.h"

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

int SysCreate(char *filename)
{
	// return value
	// 1: success
	// 0: failed
	return kernel->interrupt->CreateFile(filename);
}

void SysPrintInt(int n)
{
	//kernel->synchConsoleOut->PutChar(char(n+'0'));
	//kernel->synchConsoleOut->PutChar('\n');
	int m = 0;
 	int flag = 0;
 	char temp[64];
 	int index = 0;

 	if(n<0){
     	 	flag = 1;
      	 	n = -1*n;
 	}
 	if(n==0){
	        flag=2;
	}
 	while(n>0){
      		temp[index++]=n%10+'0';
      		n=n/10;
 	}
 	if(flag==1){
		temp[index]='-';
	}
 	else{
		index--;
	}

	int add=(int)temp;
	while(index>=0){
	    kernel->synchConsoleOut->PutChar(char(temp[index]));
	    index--;
	}
	if(flag==2)
        	kernel->synchConsoleOut->PutChar(char('0'));
	kernel->synchConsoleOut->PutChar(char('\n'));
}

OpenFileId SysOpen(char *filename)
{
	int value;
	int count = 0;
	char *paramStr = new char[128];
	do {
		kernel->machine->ReadMem((int)filename+count,1,&value);
		paramStr[count]=*(char*)&value;
		count++;
	}while(*(char*)&value!='\0'&&count<128);
	OpenFile* openfile = kernel->fileSystem->Open(paramStr);
	OpenFileId result = kernel->fileSystem->GetFiledId(paramStr);
	kernel->machine->WriteRegister(2,result);
        return result;
}

int SysWrite(char *buffer, int size, OpenFileId id){
	char* buf = new char[1];
	int count = 0;
	int ch;
	OpenFile* openfile = new OpenFile (id);

	kernel->machine->ReadMem(int(buffer),1,&ch);
	buf[0] = (char)ch;
	int result = openfile->Write(buf,size);

	kernel->machine->WriteRegister(2,result);
	//openfile->~OpenFile();
	return result;
}

int SysRead(char *buf,int size,OpenFileId id)
{
	//cout << "szie:" << size << " id:" << id << endl;
	OpenFile* openfile = new OpenFile (id);
	char temp[size];
	int readnum = 0;
	readnum = openfile->Read(temp,size);

	//delete openfile;
	kernel->machine->WriteRegister(2,readnum);
        return readnum;
}

OpenFileId SysClose(OpenFileId id){
	OpenFileId result = Close(id)+1; 
	kernel->machine->WriteRegister(2,result);
	return result;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
