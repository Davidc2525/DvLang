#include <iostream>
#include <stdlib.h>
#include "process.h"
#include <stdint.h>
#include <functional>
#include "../numbers/numbers.h"
//#include "native.cpp"

#include <unistd.h>


using namespace std;

namespace PROCESS
{

intdv * cache_ldcs = (intdv*) calloc(1000,sizeof(intdv));//crear una clase

Cpu::Cpu(int mainP, int sizeP, intdv *p, CpuArgs *args)
{
	stack_pointer = args->globals;
	initial_frame = args->globals;
	estack = new STACK::Stack(args->max_e_stack);
	 
	frame_stack = new STACK::Stack(args->max_f_stack);
	frame_stack->Push(-1);
	if (sizeP > args->max_size_program)
	{
		cout << "Max program superate " << sizeP << " size max is " << args->max_size_program << endl;
		//exit(1);
	}

	//program = new int[sizeP];
	main = mainP;
	cpu_index = args->index;
	process = args->process;
	program = p;
	ip = main;


	#if DEBUG
		cout << "CPU: main: " << main << endl;
		cout << "CPU: sizep: " << sizeP << endl;
		cout << "cpu index " << cpu_index << endl;
		for (intdv x = 0; x < sizeP; x++)
		{
			cout << program[x] << " ";
		}
	    cout << endl;
	#endif
}

void Cpu::run()
{

	while (running)
	{
		eval(next());
		#if DEBUG
			count++;
		#endif
	}
}

bool Cpu::stop()
{
	running = false;
}

intdv Cpu::next()
{
	return  program[ip++];;
}

intdv Cpu::next2()
{

	intdv x = program[ip++];
	intdv y = program[ip++];
	return y;
}

int Cpu::get_index()
{
	return cpu_index;
}

STACK::Stack * Cpu::get_e_stack(){return estack;}
STACK::Stack * Cpu::get_f_stack(){return frame_stack;}

void Cpu::eval(intdv instr)
{
	switch (instr)
	{
	default:
		cout << "INCORRECT OPCODE: "<< instr << endl;
		exit(1);
		break;
	case REALLOC:{

		intdv newSize = estack->Pop();
		intdv adr = estack->Pop();

		intdv currSize = process->heap->get(adr);

		intdv newAdr = process->heap->Realloc(adr,newSize);
		estack->Push(newAdr);

		#if DEBUG
			cout << "REALLOC; from: "<<currSize<<", to adr: "<<adr<<", newSize: "<<newSize<<", newAdr: "<<newAdr << endl;
		#endif
		break;
	}
	case FREE:{
		intdv adr = estack->Pop();
		process->heap->Free(adr);
		#if DEBUG
			cout << "FREE; adr: "<<adr << endl;
		#endif
		break;
	}
	case NEW:{
		intdv size = next();
		//process->heap->heap_pointer = process->heap->heap_pointer - size - 1;
		//process->heap->put(process->heap->heap_pointer,size);
		intdv adr = process->heap->Malloc(size);
		estack->Push(adr);
		#if DEBUG
			cout << "Allocate with NEW memory: " <<size<< " slots, HP=" <<process->heap->heap_pointer << endl;
		#endif
		break;
	}
	case IPF:{
		intdv val       =  estack->Pop();
		intdv array_adr =  estack->Pop();
		intdv index     =  next();

		process->heap->put(array_adr+1+index,val);

		//estack->Push(val);

		#if DEBUG
			cout << "IPUTFIELD: adr = "<<array_adr<<" index = "<<index<<" heap["<< array_adr+1+index <<"] val = "<<val << endl;
		#endif
		break;
	}
	case IGF:{
		//intdv val = estack->Pop();
		intdv index = next();
		intdv array_adr = estack->Pop();
		intdv val = 0;

		val = process->heap->get(array_adr+1+index);
		estack->Push(val);


		#if DEBUG
			cout << "IGETFIELD: adr = "<<array_adr<<" index = "<<index<<" heap["<< array_adr+1+index <<"], push("<<val<<")" << endl;
		#endif
		break;
	}
	case ALC:{
		
		intdv size = estack->Pop();
		//process->heap->heap_pointer = process->heap->heap_pointer - size - 1;
		//process->heap->put(process->heap->heap_pointer,size);
		//estack->Push(process->heap->heap_pointer);
		intdv adr = process->heap->Malloc(size);
		estack->Push(adr);
		#if DEBUG
			cout << "Allocate memory: " <<size<< " slots, HP=" <<process->heap->heap_pointer << endl;
		#endif
		break;
	}
	case IASTORE:{
		intdv val       =  estack->Pop();
		intdv index     =  estack->Pop();
		intdv array_adr =  estack->Pop();

		intdv a_len = process->heap->get(array_adr);
		if(index>a_len-1){
			cout << "ARRAY OVERFLOW: array length ["<<a_len<<"], index assign: "<<index<< endl;
		}

		process->heap->put(array_adr+1+index,val);

		//estack->Push(val);

		#if DEBUG
			cout << "IASTORE: adr = "<<array_adr<<" index = "<<index<<" heap["<< array_adr+1+index <<"] val = "<<val << endl;
		#endif
		break;
	}
	case IALOAD:{
		//intdv val = estack->Pop();
		intdv index = estack->Pop();
		intdv array_adr = estack->Pop();
		intdv val = 0;

		intdv a_len = process->heap->get(array_adr);
		if(index>a_len-1){
			cout << "ARRAY OVERFLOW: array length ["<<a_len<<"], index assign: "<<index<< endl;
			estack->Push(0);

		}else{
			val = process->heap->get(array_adr+1+index);
			estack->Push(val);
		}


		#if DEBUG
			cout << "IALOAD: adr = "<<array_adr<<" index = "<<index<<" heap["<< array_adr+1+index <<"], push("<<val<<")" << endl;
		#endif
		break;
	}
	case ALEN:{
		intdv array_adr = estack->Pop();
		intdv a_len = process->heap->get(array_adr);

		estack->Push(a_len);
		#if DEBUG
			cout << "ALEN: heap["<< array_adr <<"] = estack->Push() = "<< a_len<< endl;
		#endif
		break;
	}

	case LDCS:{
		//indice de la cadena en strings set
		intdv index = next();
		intdv adr = 0;
		intdv ref_in_cache = cache_ldcs[index];
		if(ref_in_cache==0){

			STRING_SET::Entry e = (*process->strings_set)[index];
			#if DEBUG
				cout << "LDCS allocate memory: " <<e.len<< " slots, str: "<<e.content<< endl;
			#endif

			//tamaño de la cadena
			intdv size = e.len;

			//se transforma la cadena de C++
			//a cadena de Dv
			//y guardamos la direccion
			adr = STRING::String::c_str_to_dv_srt(process->heap,e.content);

			cache_ldcs[index] = adr;
		}else{//in cache
			adr = ref_in_cache;
			#if DEBUG
				cout << "LDCS Ref loaded from cache: "<<adr << endl;
			#endif
		}

		estack->Push(adr);

		break;
	}
	case ICONST:
	{

		intdv v_p = next();

		estack->Push(v_p);

		#if DEBUG
			cout << "ICONST " << v_p << ", e stack pointer " << estack->getSp() << endl;
		#endif

		break;
	}
	case ADR:{
		intdv adr = next();

		estack->Push(adr);
		#if DEBUG
			cout << "ADR estack->Push(" << adr << ") " << endl;
		#endif
		break;
	}
	case ADRL:{
		intdv adr = next();

		estack->Push(frame_pointer + adr);
		#if DEBUG
			cout << "ADRL estack->Push(" << frame_pointer + adr << ") " << endl;
		#endif
		break;
	}
	//-- carga desde codigo
	case ILOADL:// LOAD LOCAL INT
	{
		intdv adr = next();
		intdv val = process->heap->get(frame_pointer + adr);

		estack->Push(val);

		#if DEBUG
			cout << "ILOADL estack->Push(heap[" << frame_pointer + adr << "]) = " << val << endl;
		#endif

		break;
	}
	case ISTOREL: //STORE LOCAL INT
	{
		intdv adr = next();
		intdv val = estack->Pop();

		process->heap->put(frame_pointer + adr, val);

		#if DEBUG
			cout << "ISTOREL heap[" << frame_pointer + adr << "] =  " << val << endl;
		#endif

		break;
	}
	case ILOAD: // load global
	{
		intdv adr = next();
		intdv val = process->heap->get(adr);

		estack->Push(val);

		#if DEBUG
			cout << "ILOAD estack->Push(heap[" << adr << "]) = " << val << endl;
		#endif
		break;
	}
	case ISTORE: // store global
	{

		intdv adr = next();
		intdv val = estack->Pop();

		process->heap->put(adr, val);

		#if DEBUG
			cout << "ISTORE heap[" << adr << "] =  " << val << endl;
		#endif

		break;
	}
	//-- carga desde stack
	case ILOAD_S: // load global
	{
		intdv adr = estack->Pop();
		intdv val = process->heap->get(adr);

		estack->Push(val);

		#if DEBUG
			cout << "ILOAD_S estack->Push(heap[" << adr << "]) = " << val << endl;
		#endif
		break;
	}
	case ISTORE_S: // store global
	{

		intdv adr = estack->Pop();
		intdv val = estack->Pop();

		process->heap->put(adr, val);

		#if DEBUG
			cout << "ISTORE_S heap[" << adr << "] =  " << val << endl;
		#endif

		break;
	}
	//
	case ADD:
	{

		intdv b = estack->Pop();
		intdv a = estack->Pop();
		intdv result = a + b;

		estack->Push(result);

		#if DEBUG
			cout << "ADD " << a << " + " << b << " push result " << result << endl;
		#endif
		break;
	}
	case SUB: 
	{

		intdv b = estack->Pop();
		intdv a = estack->Pop();
		intdv result = a - b;

		estack->Push(result);

		#if DEBUG
			cout << "SUB " << a << " - " << b << " push result " << result << endl;
		#endif
		break;
	}
	case MUL: 
	{

		intdv b = estack->Pop();
		intdv a = estack->Pop();
		intdv result = a * b;

		estack->Push(result); 

		#if DEBUG
			cout << "MUL " << a << " * " << b << " push result " << result << endl;
		#endif
		break;
	}

	case DIV:
	{
 
		intdv b = estack->Pop();
		intdv a = estack->Pop();

		if(a == 0 || b == 0){
			cout << "No se puede dividir entre cero" << endl;

			exit(1);
			break;
		}

		intdv result = a / b;

		estack->Push(result);

		#if DEBUG
			cout << "DIV " << a << " / " << b << " push result " << result << endl;
		#endif
		break;
	} 

	case MOD:
	{

		intdv b = estack->Pop();
		intdv a = estack->Pop(); 
		intdv result = a % b;

		estack->Push(result);

		#if DEBUG 
			cout << "MOD " << a << " % " << b << " push  result " << result << endl;
		#endif
		break;
	}

	case INEG:{

		intdv val = estack->Pop();
		intdv new_val;
		if(val > 0){
			new_val = -abs(val);
		}else{
			new_val = abs(val);
		}

		estack->Push(new_val);

		#if DEBUG
			cout <<"INEG " << val << " to "<< new_val<<endl;
		#endif
		break;
	}	
	case INVERT:{  
  
		intdv val = estack->Pop();
		intdv new_val;
		if(val >= 1){
			new_val = 0;
		}else{
			new_val = 1;
		}

		estack->Push(new_val);

		#if DEBUG
			cout <<"INVERT " << val << " to "<< new_val<<endl;
		#endif
		break;
	}

	case DADD:{
		long s_v_1 = estack->Pop();
		long s_v_2 = estack->Pop();

		double d_val_1 = NUMBERS::Numbers::ConverToDouble(s_v_1);
		double d_val_2 = NUMBERS::Numbers::ConverToDouble(s_v_2);

		double result = d_val_2 + d_val_1;

		long d_val = NUMBERS::Numbers::DoubleToBitLong(result);

		estack->Push(d_val);

		#if DEBUG
			cout << "DADD; "<< d_val_2<<" + "<<d_val_1 <<" = "<<result<<"; bit_val: "<<d_val << endl;
		#endif
		break;
	}

	case DSUB:{
		long s_v_1 = estack->Pop();
		long s_v_2 = estack->Pop();

		double d_val_1 = NUMBERS::Numbers::ConverToDouble(s_v_1);
		double d_val_2 = NUMBERS::Numbers::ConverToDouble(s_v_2);

		double result = d_val_2 - d_val_1;

		long d_val = NUMBERS::Numbers::DoubleToBitLong(result);

		estack->Push(d_val);

		#if DEBUG
			cout << "DSUB; "<< d_val_2<<" - "<<d_val_1 <<" = "<<result<<"; bit_val: "<<d_val << endl;
		#endif
		break;
	}

	case DMUL:{
		long s_v_1 = estack->Pop();
		long s_v_2 = estack->Pop();

		double d_val_1 = NUMBERS::Numbers::ConverToDouble(s_v_1);
		double d_val_2 = NUMBERS::Numbers::ConverToDouble(s_v_2);

		double result = d_val_2 * d_val_1;

		long d_val = NUMBERS::Numbers::DoubleToBitLong(result);

		estack->Push(d_val);

		#if DEBUG
			cout << "DMUL; "<< d_val_2<<" * "<<d_val_1 <<" = "<<result<<"; bit_val: "<<d_val << endl;
		#endif
		break;
	}

	case DDIV:{
		long s_v_1 = estack->Pop();
		long s_v_2 = estack->Pop();

		double d_val_1 = NUMBERS::Numbers::ConverToDouble(s_v_1);
		double d_val_2 = NUMBERS::Numbers::ConverToDouble(s_v_2);

		if(d_val_1 == 0 || d_val_2 == 0){
			estack->Push(0);
			#if DEBUG
				cout << "Divicion entre 0 no permitido; push 0" << endl;
			#endif
			break;
		}

		double result = d_val_2 / d_val_1;

		long d_val = NUMBERS::Numbers::DoubleToBitLong(result);

		estack->Push(d_val);

		#if DEBUG
			cout << "DDIV; "<< d_val_2<<" / "<<d_val_1 <<" = "<<result<<"; bit_val: "<<d_val << endl;
		#endif
		break;
	}
	case DMOD:{
		long s_v_1 = estack->Pop();
		long s_v_2 = estack->Pop();

		double d_val_1 = NUMBERS::Numbers::ConverToDouble(s_v_1);
		double d_val_2 = NUMBERS::Numbers::ConverToDouble(s_v_2);

		double result = fmod(d_val_2,d_val_1);

		long d_val = NUMBERS::Numbers::DoubleToBitLong(result);

		estack->Push(d_val);

		#if DEBUG
			cout << "DMOD; "<< d_val_2<<" % "<<d_val_1 <<" = "<<result<<"; bit_val: "<<d_val << endl;
		#endif
		break;
	}

	case DNEG:{
		long s_v_1 = estack->Pop();

		double d_val_1 = NUMBERS::Numbers::ConverToDouble(s_v_1);

		double result =  0.0;

		if(d_val_1 > 0.0){
			result = -d_val_1;
		}else{
			result = d_val_1;
		}

		long d_val = NUMBERS::Numbers::DoubleToBitLong(result);

		estack->Push(d_val);

		#if DEBUG
			cout << "DNEG; -("<< d_val_1<<")  = "<<result<<"; bit_val: "<<d_val << endl;
		#endif
		break;
	}

	//float
	case FADD:{
		int s_v_1 = estack->Pop();
		int s_v_2 = estack->Pop();

		float d_val_1 = NUMBERS::Numbers::ConverToFloat(s_v_1);
		float d_val_2 = NUMBERS::Numbers::ConverToFloat(s_v_2);

		float result = d_val_2 + d_val_1;

		int d_val = NUMBERS::Numbers::FloatToBitInt(result);

		estack->Push(d_val);

		#if DEBUG
			cout << "FADD; "<< d_val_2<<" + "<<d_val_1 <<" = "<<result<<"; bit_val: "<<d_val << endl;
		#endif
		break;
	}

	case FSUB:{
		int s_v_2 = estack->Pop();
		int s_v_1 = estack->Pop();

		float d_val_1 = NUMBERS::Numbers::ConverToFloat(s_v_1);
		float d_val_2 = NUMBERS::Numbers::ConverToFloat(s_v_2);

		float result = d_val_2 - d_val_1;

		int d_val = NUMBERS::Numbers::FloatToBitInt(result);

		estack->Push(d_val);

		#if DEBUG
			cout << "FSUB; "<< d_val_2<<" - "<<d_val_1 <<" = "<<result<<"; bit_val: "<<d_val << endl;
		#endif
		break;
	}

	case FMUL:{
		int s_v_2 = estack->Pop();
		int s_v_1 = estack->Pop();

		float d_val_1 = NUMBERS::Numbers::ConverToFloat(s_v_1);
		float d_val_2 = NUMBERS::Numbers::ConverToFloat(s_v_2);

		float result = d_val_2 * d_val_1;

		int d_val = NUMBERS::Numbers::FloatToBitInt(result);

		estack->Push(d_val);

		#if DEBUG
			cout << "FMUL; "<< d_val_2<<" * "<<d_val_1 <<" = "<<result<<"; bit_val: "<<d_val << endl;
		#endif
		break;
	}

	case FDIV:{
		int s_v_2 = estack->Pop();
		int s_v_1 = estack->Pop();

		float d_val_1 = NUMBERS::Numbers::ConverToFloat(s_v_1);
		float d_val_2 = NUMBERS::Numbers::ConverToFloat(s_v_2);

		if(d_val_1 == 0 || d_val_2 == 0){
			estack->Push(0);
			#if DEBUG
				cout << "Divicion entre 0 no permitido; push 0" << endl;
			#endif
			break;
		}

		float result = d_val_2 / d_val_1;

		int d_val = NUMBERS::Numbers::FloatToBitInt(result);

		estack->Push(d_val);

		#if DEBUG
			cout << "FDIV; "<< d_val_2<<" / "<<d_val_1 <<" = "<<result<<"; bit_val: "<<d_val << endl;
		#endif
		break;
	}
	case FMOD:{
		int s_v_2 = estack->Pop();
		int s_v_1 = estack->Pop();

		float d_val_1 = NUMBERS::Numbers::ConverToFloat(s_v_1);
		float d_val_2 = NUMBERS::Numbers::ConverToFloat(s_v_2);

		double result = fmod(d_val_2,d_val_1);

		int d_val = NUMBERS::Numbers::FloatToBitInt(result);

		estack->Push(d_val);

		#if DEBUG
			cout << "FMOD; "<< d_val_2<<" % "<<d_val_1 <<" = "<<result<<"; bit_val: "<<d_val << endl;
		#endif
		break;
	}

	case FNEG:{
		int s_v_1 = estack->Pop();

		float d_val_1 = NUMBERS::Numbers::ConverToFloat(s_v_1);

		float result =  0.0;

		if(d_val_1 > 0.0){
			result = -d_val_1;
		}else{
			result = d_val_1;
		}

		int d_val = NUMBERS::Numbers::FloatToBitInt(result);

		estack->Push(d_val);

		#if DEBUG
			cout << "FNEG; -("<< d_val_1<<")  = "<<result<<"; bit_val: "<<d_val << endl;
		#endif
		break;
	}
	//


	case IWRITE:
	{

		intdv a = estack->Pop();
		cout << a <<endl;

		#if DEBUG
			cout << "IWRITE " << a << endl;
		#endif
		break;
	}
	case FWRITE:
	{

		float a = NUMBERS::Numbers::ConverToFloat(estack->Pop());
		cout << a <<endl;

		#if DEBUG
			cout << "FWRITE " << a << endl;
		#endif
		break;
	}

	case DWRITE:
	{

		double a = NUMBERS::Numbers::ConverToDouble(estack->Pop());
		//cout << a <<endl;
		printf("%.31lf \n", a);

		#if DEBUG
			cout << "DWRITE " << a << endl;
		#endif
		break;
	}

	case FTOD:{
		int s_v = estack->Pop();
		float val = NUMBERS::Numbers::ConverToFloat(s_v);
		double d_val = (double) val;

		long bit_val = NUMBERS::Numbers::DoubleToBitLong(d_val);

		estack->Push(bit_val);
		#if DEBUG
			cout << "FTOD; stack val: "<<s_v<<", float val: "<< val<<", double val: "<<d_val<<", bit_val: "<<bit_val << endl;
		#endif
		break;
	}
	case DTOF:{
		long s_v = estack->Pop();
		double val = NUMBERS::Numbers::ConverToDouble(s_v);
		float f_val = (float) val;

		int bit_val = NUMBERS::Numbers::FloatToBitInt(f_val);
		estack->Push(bit_val);
		#if DEBUG
			cout << "DTOF; stack val: "<<s_v<<", double val: "<< val<<", float val: "<<f_val<<", bit_val: "<<bit_val << endl;
		#endif
		break;
	}
	case DTOI:{
		long s_v = estack->Pop();
		double val = NUMBERS::Numbers::ConverToDouble(s_v);
		int i_val = (int) val;

		estack->Push(i_val);
		#if DEBUG
			cout << "DTOI; stack val: "<<s_v<<", double val: "<< val<<", int val: "<<i_val<< endl;
		#endif
		break;
	}
	case FTOI:{
		int s_v = estack->Pop();
		float val = NUMBERS::Numbers::ConverToFloat(s_v);
		int i_val = (int) val;

		estack->Push(i_val);
		#if DEBUG
			cout << "FTOI; stack val: "<<s_v<<", float val: "<< val<<", int val: "<<i_val<< endl;
		#endif
		break;
	}
	case ITOD:{

		int val = estack->Pop();
		long bit_val = NUMBERS::Numbers::DoubleToBitLong((double)val);

		estack->Push(bit_val);
		#if DEBUG
			cout << "ITOD; stack val: "<<val<<", bit val: "<< bit_val<< endl;
		#endif
		break;
	}
	case ITOF:{

		int val = estack->Pop();
		int bit_val = NUMBERS::Numbers::FloatToBitInt(val);

		estack->Push(bit_val);
		#if DEBUG
			cout << "ITOF; stack val: "<<val<<", bit val: "<< bit_val<< endl;
		#endif
		break;
	}
	case DTOS:{
		long val_int = estack->Pop();
		double val_do = NUMBERS::Numbers::ConverToDouble(val_int);
		char * chars_val;
		sprintf(chars_val,"%.7lf",val_do);
		//string str_val = string(chars_val);// to_string(val_do);
		intdv adr = STRING::String::c_str_to_dv_srt(process->heap,chars_val);

		/*int heap_size = str_val.length();
		intdv adr = process->heap->Calloc(heap_size);

		for(int x = 0;x < heap_size; x++){
				#if DEBUG
					int index_char = adr+1+x;
					char str_char = (int)str_val[x];
					process->heap->put(index_char,(int)str_char);
					cout << " Put char heap["<<index_char<<"] = "<<str_char<< " -> " << (int)str_char << endl;
				#else
					process->heap->put(adr+1+x,(int)str_val[x]);
				#endif
			}
*/
		estack->Push(adr);

		#if DEBUG
			cout << "DTOS; double_val: "<<val_do<<", str_val: '"<<chars_val<<"'" << endl;
		#endif
		break;
	}
	case FTOS:{
		intdv val_int = estack->Pop();
		float val_flo = NUMBERS::Numbers::ConverToFloat(val_int);
		string str_val = to_string(val_flo);
		intdv adr = STRING::String::c_str_to_dv_srt(process->heap,str_val);

		/*int heap_size = str_val.length();
		intdv adr = process->heap->Calloc(heap_size);

		for(int x = 0;x < heap_size; x++){
				#if DEBUG
					int index_char = adr+1+x;
					char str_char = (int)str_val[x];
					process->heap->put(index_char,(int)str_char);
					cout << " Put char heap["<<index_char<<"] = "<<str_char<< " -> " << (int)str_char << endl;
				#else
					process->heap->put(adr+1+x,(int)str_val[x]);
				#endif
			}
		*/
		estack->Push(adr);

		#if DEBUG
			cout << "FTOS; float_val: "<<val_flo<<", str_val: '"<<str_val<<"'" << endl;
		#endif
		break;
	}
	case ITOS:{
		intdv val_int = estack->Pop();

		string str_val = to_string(val_int);
		#if DEBUG
			cout << "ITOS; int_val: "<<val_int<<", str_val: '"<<str_val<<"'" << endl;
		#endif
		intdv adr = STRING::String::c_str_to_dv_srt(process->heap,str_val);
		/*int heap_size = str_val.length();
		intdv adr = process->heap->Calloc(heap_size);


		for(int x = 0;x < heap_size; x++){
				#if DEBUG
					int index_char = adr+1+x;
					char str_char = (int)str_val[x];
					process->heap->put(index_char,(int)str_char);
					cout << " Put char heap["<<index_char<<"] = "<<str_char<< " -> " << (int)str_char << endl;
				#else
					process->heap->put(adr+1+x,(int)str_val[x]);
				#endif
		}*/

		estack->Push(adr);

		break;
	}
	case STRCAT:{

		intdv adr_b = estack->Pop();
        intdv adr_a = estack->Pop();

		int a_len = process->heap->get(adr_a);
		int b_len = process->heap->get(adr_b);
		int new_len = b_len + a_len;

		char * str_a = STRING::String::dv_str_to_c_srt(process->heap,adr_a);
		char * str_b = STRING::String::dv_str_to_c_srt(process->heap,adr_b);
		/* char * str_concat = (char*) malloc(new_len*sizeof(char*));
		 */
		string str_concat = string (str_a).append(str_b);
		//intdv adr = process->heap->Calloc(new_len);

		/* for(int x = 0; x < a_len;x++){
			str_concat[x] = str_a[x];
		}

		for(int x = 0; x < b_len;x++){
			str_concat[a_len+x] = str_b[x];
		} */

		intdv adr = STRING::String::c_str_to_dv_srt(process->heap,str_concat);

		#if DEBUG
			cout << "STRCAT; '"<<str_a<<"' + '"<<str_b<<"' = '" <<str_concat<<"', adr = "<<adr<< endl;
		#endif

		/*for(int x = 0;x < new_len; x++){
				#if DEBUG
					int index_char = adr+1+x;
					char str_char = (int)str_concat[x];
					process->heap->put(index_char,(int)str_char);
					cout << " Put char heap["<<index_char<<"] = "<<str_char<< " -> " << (int)str_char << endl;
				#else
					process->heap->put(adr+1+x,(int)str_concat[x]);
				#endif
		}*/

		estack->Push(adr);

		free(str_a);
		free(str_b);
 
		break;
	}
	case CWRITE:
	{

		/*intdv len = next();
		char c[len];
		for (intdv x = len - 1; x >= 0; x--)
		{
			c[x] = (char)estack->Pop();
		}*/

		/*for (intdv y = 0; y < len ; y++){
		        cout<<c[y];
		    }*/
		/*intdv index = estack->Pop();
		string c = process->strings_set->get(index).content;*/

		string c = "";
		int len = estack->Pop();//process->heap->get(adr);
		intdv adr = estack->Pop();
		for (intdv x = 0;x < len;x++)
		{
			c += (char)process->heap->get(adr+1+x);
		}

		#if DEBUG
			cout<<"CWRITE strings["<<adr<<"]; len: "<<len<<"; \""<<c<<"\""<<endl;
		#else
			cout << c;
		#endif
		break;
	}

	

	case CMPI://comparacion de enteros
	{	


		intdv b = estack->Pop();
		intdv a = estack->Pop();

		this->flag[0] = (a == b)? 1 : 0;
		this->flag[1] = (a > b)? 1 : 0;
		

		#if DEBUG
			cout<< "CMP; flag[0] = "<<this->flag[0] << " flag[1] = "<<this->flag[1] << endl;
		#endif

		break;
	}
	case JMP: {		
			ip = next();
		#if DEBUG
			cout<< "JMP;  jump to: "<<ip<< endl;
		#endif
		break;
	}
	case JE: {
		if(this->flag[0]==1){
			ip = next();
		}else{
			ip = ip + 1;
		}
		#if DEBUG
			cout<< "JE; a == b "<<this->flag[0]<< endl;
		#endif
		break;
	}
	case JNE: {
		if(this->flag[0]==0){
			ip = next();
		}else{
			ip = ip + 1;
		}
		#if DEBUG
			cout<< "JNE; a != b "<<(this->flag[0]==0)<< endl;
		#endif
		break;

	}
	case JG: {
		if(this->flag[1]==1){
			ip = next();
		}else{
			ip = ip + 1;
		}
		#if DEBUG
			cout<< "JG; a > b "<<(this->flag[1]==1)<< endl;
		#endif
		break;
	}
	case JL: {
		if(this->flag[1]==0){
			ip = next();
		}else{
			ip = ip + 1;
		}
		#if DEBUG
			cout<< "JL; a < b "<<(this->flag[1]==0)<< endl;
		#endif
		break;	
	}
	case JGE: {
		if(this->flag[0]==1||this->flag[1]==1){
			ip = next();
		}else{
			ip = ip + 1;
		}
		#if DEBUG
			cout<< "JGE; a >= b "<<(this->flag[0]==1||this->flag[1]==1)<< endl;
		#endif
		break;
	}
	case JLE: {
		if(this->flag[0]==1||this->flag[1]==0){
			ip = next();
		}else{
			ip = ip + 1;
		}
		#if DEBUG
			cout<< "JLE; a <= b "<<(this->flag[0]==1||this->flag[1]==0)<< endl;
		#endif
		break;
	}

	//
	case SETE: {
		
		this->estack->Push(this->flag[0]==1);

		#if DEBUG
			cout<< "SETE; a == b "<<this->flag[0]<< endl;
		#endif
		break;
	}
	case SETNE: {
		
		this->estack->Push(this->flag[0]==0);

		#if DEBUG
			cout<< "SETNE; a != b "<<(this->flag[0]==0)<< endl;
		#endif
		break;

	}
	case SETG: {
		
		this->estack->Push(this->flag[1]==1);

		#if DEBUG
			cout<< "SETG; a > b "<<(this->flag[1]==1)<< endl;
		#endif
		break;
	}
	case SETL: {
		
		this->estack->Push(this->flag[1]==0);

		#if DEBUG
			cout<< "SETL; a < b "<<(this->flag[1]==0)<< endl;
		#endif
		break;	
	}
	case SETGE: {

		this->estack->Push(this->flag[0]==1||this->flag[1]==1);

		#if DEBUG
			cout<< "SETGE; a >= b "<<(this->flag[0]==1||this->flag[1]==1)<< endl;
		#endif
		break;
	}
	case SETLE: {

		this->estack->Push(this->flag[0]==1||this->flag[1]==0);

		#if DEBUG
			cout<< "SETLE; a <= b "<<(this->flag[0]==1||this->flag[1]==0)<< endl;
		#endif
		break;
	}
	//

	case AND:
	{
		intdv b = estack->Pop();
		intdv a = estack->Pop();

		estack->Push(a & b);

		#if DEBUG
			cout << "AND " << a << " && " << b << " " << (a & b == 1 ? "true" : "false") << endl;
		#endif
		break;
	}
	case OR:
	{
		intdv b = estack->Pop();
		intdv a = estack->Pop();

		estack->Push(a | b);

		#if DEBUG
			cout << "OR = " << a << " || " << b << " " << (a | b == 1 ? "true" : "false") << endl;
		#endif
		break;
	}
	case NCALL:{ 
		int native_id = estack->Pop();
		
		//llamadas a funciones nativas
		#if DEBUG
			cout << "llamando funcion nativa: "<< native_id << endl;
		#endif
		Cpu * c = this;
	    //intdv val = native->call_wrap2<intdv>(native_id,c);
		 
		intdv val = process->libload->call(native_id,c);

		estack->Push(val);
		#if DEBUG
			cout << "devolviendo el control al programa resultado: "<<val << endl;
		#endif
		break;
	}
	/**
	 * llamada a sub rutina de manera asincrona (corrutina)
	 * de la pila de expresiones actual se cargan "n_pars" elementos
	 * q luego se introduciran en la nueva pila de expresiones
	 * del nuevo cpu
	 * */
	case CO_CALL:{//TODO

		intdv main = estack->Pop();//donde comensara a ejecutarse la nueva rutina

		int n_pars = program[main+1];


		PROCESS::Args *processArgs = new PROCESS::Args();
		//process args
		processArgs->max_heap = process->get_default_args()->max_heap;
		processArgs->max_e_stack = process->get_default_args()->max_e_stack;
		processArgs->max_f_stack = process->get_default_args()->max_f_stack; // TODO delete
		processArgs->max_size_program = process->get_default_args()->max_size_program;
		//cpu args
		processArgs->main = main;
		processArgs->sizeP = process->get_default_args()->sizeP;
		processArgs->program = process->get_default_args()->program;

		int cpuid = process->fork(processArgs);
		Cpu * cpu = process->get_cpu(cpuid);

		intdv * params = new intdv[n_pars];

		#if DEBUG
			cout << "CO_CALL; new cpuid  "<< cpuid << "; n° params: "<<n_pars<<endl;
			cout << " CO_CALL; transfer n° params: "<<n_pars<<""<<endl;
		#endif

		for (int x =n_pars-1;x>=0;x--)
		{

			params[x] = estack->Pop();;

		}

		for (int x =0;x<n_pars;x++)
		{
			intdv val = params[x];
			//process->heap->put(frame_pointer + i, val);
			cpu->get_e_stack()->Push(val);
			#if DEBUG
			cout << " CO_CALL; push param: "<<val << endl;
			#endif
		}
		delete[] params;
		process->init_cpu(cpuid);
		break;
	}
	case CALL://deprecated; see CALL_S
	{
		
		frame_stack->Push(ip + 1);
		intdv n_ip = next();


		#if DEBUG
			cout << "CALL " << n_ip << endl;
		#endif


		ip = n_ip;
		break;
	}
	/**
	 * saltar a direcion de funcion,
	 * la direccion la obtiene de la pila
	*/
	case CALL_S:
	{

		frame_stack->Push(ip);
		//cout<<"CALL: "<<ip<<endl;
		intdv n_ip = estack->Pop();


		#if DEBUG
			cout << "CALL_S new ip from stack " << n_ip << endl;
		#endif


		ip = n_ip;
		break;
	}
	
	case ENTER:
	{
		//intdv _ip = main + 1 == ip ? main : estack->Pop();
		intdv nPars = next();
		intdv nVars = next();
		#if DEBUG
			cout << "ENTER "<<ip<<" " << nPars <<" PARAMS"<<" VARS: "<<nVars<< endl;
		#endif

		intdv current_frame = frame_pointer;
		frame_stack->Push(frame_pointer);
		frame_pointer = stack_pointer;
		stack_pointer = stack_pointer +  nVars;

		#if DEBUG 
			cout << "cfp: "<<current_frame<<" nfp: "<<frame_pointer << endl;
		#endif

		for (intdv i = nPars - 1; i >= 0; i--)
		{
			intdv val = estack->Pop();
			process->heap->put(frame_pointer + i, val);
			#if DEBUG
			cout << " ENTER; SAVE PARAM: heap[" << (frame_pointer + i) << "]  =  estack->Pop() = " << val << endl;
			#endif
		}

		//frame_stack->Push(_ip);

		break;
	}
	case EXIT:{
		//TODO
		#if DEBUG
			//cout << "EXITING " << endl;
		#endif
		stack_pointer = frame_pointer;
		frame_pointer = frame_stack->Pop();
		//cout << "EXIT; " << frame_pointer << endl;
		#if DEBUG
			cout << "EXIT; c fp: "<<stack_pointer<<" n fp " << frame_pointer << endl;
		#endif
		break;
	}
	case RET:
	{
		//frame_pointer = frame_stack->Pop();
		intdv p_ip = frame_stack->Pop();
		if(p_ip != -1){
			#if DEBUG
				cout <<"RET ip "<< ip << endl;
			#endif
			ip = p_ip;
		}
		  
		break;
	}
	case DUP:{
		intdv val = estack->Pop();
		estack->Push(val);
		estack->Push(val);
		#if DEBUG
			cout << "DUP estack->pop() = "<<val<<"; estack->push("<<val<<"), estack->push("<<val<<")"  << endl;
		#endif
		break;
	}
	case POP:{

		#if DEBUG
			cout << "POP; "<<estack->Pop()<< endl;
		#else
			estack->Pop();
		#endif

		break;
	}
	case HLT:
		running = false;
		#if DEBUG
			cout << "HLT " << endl;
		#endif
		break;
	}

}

Cpu::~Cpu() {
	//TODO
}

} // namespace PROCESS
