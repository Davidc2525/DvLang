

#include <iostream>
#include <stdlib.h>
#include "heap.h"
#include <array>
using namespace std;

namespace Heap
{

Heap::Heap(intdv heapSize)
{
    heap = (intdv *)calloc(heapSize , sizeof(intdv*)); //new intdv[heapSize];
    if (heap == NULL)
    {
        cout << "No se pudo apartar la memoria;" << endl;
        exit(1);
    }
    freedons = new Freedons(100);
    MAX_HEAP = heapSize;
    //heap_pointer = heapSize;
}

Heap::~Heap()
{
}

void Heap::put(intdv adr, intdv val)
{
    //cout << "PUT" <<adr<<" "<< MAX_HEAP<<" "<<(adr > MAX_HEAP)<< endl;
    if ((adr > MAX_HEAP))
    {

        //cout << "PUT" <<adr<<" "<< MAX_HEAP<< endl;
        //MAX_HEAP = MAX_HEAP + adr;
        #if DEBUG
            cout << "HEAP OVERFLOW " << MAX_HEAP - adr << " ADR " << adr << endl;
            cout << "REALLOC HEAP: " << MAX_HEAP - adr << " TO: " << MAX_HEAP << endl;
        #endif

        //heap = (intdv *) realloc(heap,MAX_HEAP*sizeof(intdv));

        int index = 0; 
        int y = 0;
        for (int x = heap_pointer; x < MAX_HEAP / 8; x++)
        {
            if(y==0){
               // cout << index << ": ";
            }
            
            cout << heap[x] << " ";
            if(y<8){
                y++;
                if(y==8){
                    //cout<<endl;
                    y=0;
                }
            }

            index = x + 8;
            if (x == (adr))
                break;
        }
        //exit(1);
    }
    //cout << "return "<<heap<<" "<<adr<<" " <<val<< endl;
    heap[adr] = val;
}

intdv Heap::get(intdv adr)
{
    if (adr > MAX_HEAP)
    {
        cout << "MAX HEAP ERROR " << adr << endl;
        int index = 0;
        for (int x = 0; x < MAX_HEAP / 8; x++)
        {

            char h[16];
            cout << index << ": ";
            for (int y = 0; y < 8; y++)
            {
                cout << heap[index + y] << " ";
                if (y == 7)
                    cout << endl;
            }

            index = x + 8;
            if (x == adr)
                break;
        }

        exit(1);
    }
    return heap[adr];
}
 
class Filtered
{
  public:
    Freedon **frdons; 
    int len;
};

Filtered * filterSize(Freedons *freedons, intdv size)
{
    Freedon **frdons = (Freedon **)calloc(10 , sizeof(Freedon*));
    unsigned int y = 0;
    for (unsigned int x = 0; x < freedons->_size; x++)
    {
        
        //cout << x << endl;
        if (freedons->get(x) != NULL)
        {

            if (freedons->get(x)->len > size + 1)
            {
                frdons[y] = freedons->get(x);
                y++;
            }
            #if DEBUG
                cout<<"  " << freedons->get(x)->toString() << endl;
            #endif
        }
    }
    Filtered * filtered = new Filtered();
    filtered->frdons = frdons;
    filtered->len = y;
    return filtered;
}

intdv Heap::Realloc(intdv adr, intdv newSize) { 
    if(newSize == heap[adr]){
        return adr;
    }
    intdv newAdr = Calloc(newSize);
    //Free(adr);
    if(newSize > heap[adr]){
        for(intdv x = 0;x < heap[adr]; x++){
            heap[newAdr+1+x] = heap[adr+1+x];
            heap[adr+1+x] = NULL;
        }
    }else{
        for(intdv x = 0;x < heap[adr]; x++){
            if(x<newSize){
                heap[newAdr+1+x] = heap[adr+1+x];
            }
            heap[adr+1+x] = NULL;
        }
    }
    Free(adr);
    heap[adr] = NULL;
    return newAdr; 
};
intdv Heap::Calloc(intdv size) { 
    return Fill(Malloc(size),0);
};
intdv Heap::Malloc(intdv size)
{
    #if DEBUG
        cout << "   malloc: "<<size << endl;
    #endif
    Freedon * freeInMemo = SearchInFree(size);
    if(freeInMemo != NULL){
        //cout << "freeInMemo != NULL: "<<(freeInMemo != NULL) << endl;
        int index = FindByFrom(freeInMemo->from);
        Freedon * f = freedons->get(index);

      //cout << "f->from + size +1 < MAX_HEAP: "<<(f->from + size +1 < heap_pointer) << endl;
        if(f->from + size + 1 < MAX_HEAP){
            
            freedons->remove(index);
            
            Freedon * nfreedon = new Freedon();            
            
            nfreedon->from = f->from + size + 1;
            nfreedon->to = f->to;
            nfreedon->len = f->len - (size+1);
            
            freedons->set(index,nfreedon);

            heap[f->from] = size;
            #if DEBUG
                cout << "   ALOCATE WITH FREEDON SPACE; adr: "<<f->from<<" size: "<<size << endl;
            #endif
            intdv adr = f->from;
            //free(f); 
            return adr;
        }
    }

    if (size < heap_pointer && (heap_pointer - size - 1) > 0)
    {
        heap_pointer = heap_pointer - size - 1;
        heap[heap_pointer] = size;
        #if DEBUG
                cout << "   ALOCATE WITH HEAP POINTER; adr: "<<heap_pointer<<" size: "<<size << endl;
        #endif
    }
    else
    {
         cout<<"heap overflow, cant allocate: "<< size << endl;
         exit(1);
    }
    return heap_pointer;
};

Freedon * Heap::SearchInFree(intdv size)
{
    intdv indexMin = -1;
    int min = INT32_MAX;
    Filtered * f = filterSize(freedons,size);

    if(f->len > 0){
        if(f->len == 1){
            return f->frdons[0];
        }
        if(f->len > 1){

            //encotrar el de menor espacio en memoria
            for(unsigned int x = 0;x<f->len;x++){
                if(min > f->frdons[x]->len){
                    min = f->frdons[x]->len;
                    indexMin=x;
                }
            }
            free(*f->frdons);
            free(f);
            return f->frdons[indexMin];
        }else{
            return NULL;
        }
    }
};
void Heap::Free(intdv adr){
    intdv len  = heap[adr];
    intdv from = adr;
    intdv to   = from + len + 1;

    #if DEBUG
        cout << "   free: from: "<<adr <<", to: "<<to<<", len: "<<(len+1)<< endl;
    #endif
    bool foundContiguoFree = false;

    int x = 0;

    //contiguo hacia abajo
    while(true){
        if(freedons->size > 0){
            Freedon * f = freedons->get(x);
            //cout << "IS NULL: "<<(f==NULL)<<endl;

            if(f!=NULL){
                //cout<<"to == f->from "<<(to == f->from)<<endl;
                if(to == f->from){
                    #if DEBUG
                        cout << "   contiguo abajo: from: "<<(f->from )<<", to: "<<(f->to)<<", len: "<<(f->len)<< endl;
                    #endif
                    foundContiguoFree = true;
                    Freedon * nfreedon = new Freedon();

                    nfreedon->from = from;
                    nfreedon->to = f->to;
                    nfreedon->len = f->len + 1 + len;
                   
                    freedons->set(x,nfreedon);

                    #if DEBUG
                        cout << "   contiguo abajo: puting:"<<nfreedon->toString()<< endl;
                    #endif


                    heap[from] = NULL;

                    to = f->to;
                }
            }

            if(x > freedons->size)
                break;
            x++;
        }else{
            break;
        }
    }  

    //congiguo hacia arriba
    x = freedons->size-1;
    while(true){
        
        if(freedons->size > 0){
            Freedon * f = freedons->get(x);
            
            if(f != NULL){
                if(f->to == from){
                    #if DEBUG
                        cout << "   contiguo arriba: from: "<<(f->from )<<", to: "<<(f->to)<<", len: "<<(f->len)<< endl;
                    #endif
                    foundContiguoFree = true;
                    heap[from] = NULL;
                    from = f->from;

                    Freedon * nfreedon = new Freedon();

                    nfreedon->from = from;
                    nfreedon->to = to;
                    nfreedon->len = to - from;

                    freedons->set(x,nfreedon);
                    #if DEBUG
                        cout << "   contiguo arriba: puting:"<<nfreedon->toString()<< endl;
                    #endif
                    if(freedons->size>1){
                        intdv d = FindByFrom(adr);
                        if(d!=-1){
                            freedons->remove(d);
                        }
                    }
                }
            }
            if(x<=0)
                break;
            x--;
        }else{
            break;
        }
    }

    if(!foundContiguoFree){
        #if DEBUG
            cout << "   SIN CONTIGUO" << endl;
        #endif
        Freedon * nfreedon = new Freedon();

        nfreedon->from = from;
        nfreedon->to = to;
        nfreedon->len = len + 1;
        freedons->set(freedons->lastIndex,nfreedon);
        freedons->lastIndex++;
        #if DEBUG
            cout << "   FREE: from: "<<adr <<", to: "<<to<<", len: "<<(len+1)<< endl;
            cout <<"    PUT: "<<freedons->lastIndex<<" "<< nfreedon->toString() << endl;
        #endif
    }  
};

intdv Heap::FindByFrom(intdv adr) { 
    int index = -1;
    for(unsigned int x =0;x<freedons->size;x++){
        if(freedons->get(x)!=NULL){
            if(freedons->get(x)->from == adr){
                    index = x;
            }
        }
    }
    return index;
};

intdv Heap::Fill(intdv adr, intdv fillWith) {
     int len = heap[adr];
     for(intdv x = 0; x < len;x++){
         heap[adr+1+x] = fillWith;
     }    
     return adr; 
}; 

intdv Heap::HeapFree() { return 0; };

} // namespace Heap
