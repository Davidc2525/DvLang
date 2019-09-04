#include "strings.h"
#include <string.h>

char* dv_str_to_c_srt(Heap::Heap *heap, intdv adr){
    int str_len = heap->get(adr);
	#if DEBUG
		cout << "STRING::String::dv_str_to_c_srt; adr: "<<adr<< ", size: "<<str_len<< endl;
	#endif 
	char * str = (char*) malloc(str_len*sizeof(char*));
	for(int x = 0;x<str_len;x++){
		#if DEBUG
			char c = (char) heap->get(adr+1+x);			
			cout << " str["<< x <<"] = heap["<<adr+1+x<<"] ("<< c <<")"<< endl;
			str[x] = c;
		#else
			str[x] = (char) heap->get(adr+1+x);
		#endif
	}

	return str;
};
  
intdv c_str_to_dv_srt(Heap::Heap *heap, char* c_str){
    int size = strlen(c_str);
    #if DEBUG
		cout << "c_str_to_dv_srt; str: "<<c_str<<" strl: "<<size<<endl;
	#endif
    string str(c_str);
    intdv adr = heap->Calloc(size+1);

    for(int x = 0;x < size; x++){ 
        #if DEBUG
            int index_char = adr+1+x;
            char str_char = str[x];
            heap->put(index_char,(int)str_char);
            cout << " Put char heap["<<index_char<<"] = "<<str_char<< " -> " << (int)str_char << endl;			 	
        #else
            heap->put(adr+1+x,(int)str[x]); 
        #endif
    }
    //heap->put(adr+1+size-1,0);
    #if DEBUG
        cout << " c_str_to_dv_srt adr: "<<adr << endl;
    #endif
        
    return adr;
};

//----------------- CLASS MEMBERS
using namespace STRING;
char* String::dv_str_to_c_srt(Heap::Heap *heap, intdv adr){
    int str_len = heap->get(adr);
	#if DEBUG
		cout << "STRING::String::dv_str_to_c_srt; adr: "<<adr<< ", size: "<<str_len<< endl;
	#endif
	char * str = (char*) malloc(str_len*sizeof(char*));
	for(int x = 0;x<str_len;x++){
		#if DEBUG
			char c = (char) heap->get(adr+1+x);			
			cout << " str["<< x <<"] = heap["<<adr+1+x<<"] ("<< c <<")"<< endl;
			str[x] = c;
		#else
			str[x] = (char) heap->get(adr+1+x);
		#endif
	}

	return str;
};


intdv String::c_str_to_dv_srt(Heap::Heap *heap, char *str){
    return String::c_str_to_dv_srt(heap, string(str));
};

intdv String::c_str_to_dv_srt(Heap::Heap *heap, const char *str){
    return String::c_str_to_dv_srt(heap,(char*) str);
};

intdv String::c_str_to_dv_srt(Heap::Heap *heap, string str){
    #if DEBUG
		cout << "String::c_str_to_dv_srt; str: "<<str<<endl;
	#endif
        
    int size = str.length();
    intdv adr = heap->Calloc(size+1);//el +1 esta pendiente !!!!

    for(int x = 0;x < size; x++){
        #if DEBUG
            int index_char = adr+1+x;
            char str_char = str[x];
            heap->put(index_char,(int)str_char);
            cout << " Put char heap["<<index_char<<"] = "<<str_char<< " -> " << (int)str_char << endl;			 	
        #else
            heap->put(adr+1+x,(int)str[x]);
        #endif
    }
    //heap->put(adr+1+size-1,0);
    #if DEBUG
        cout << " c_str_to_dv_srt adr: "<<adr << endl;
    #endif
        
    return adr;
};
