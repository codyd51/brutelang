#import <stdio.h>
#import <stdlib.h>

#define STACK_MAX 256
#define INITIAL_GC_THRESHOLD 1000

typedef enum {
	OBJ_INT,
	OBJ_FLOAT,
	OBJ_STRING,
	OBJ_PAIR
} ObjectType;

typedef struct sObject {
	//each object retains a reference to the next in a linked list
	//this is so we can walk through and find all unmarked objects to be collected in GC, even if the reference is lost to the language
	struct sObject* next;

	//marker for check-sweep garbage collection
	unsigned char marked;
	
	ObjectType type;
	
	union {
		//OBJ_INT
		int value;

		//OBJ_FLOAT
		float fvalue;
		
		//OBJ_STRING
		const char* text;
		
		//OBJ_PAIR
		struct {
			struct sObject* head;
			struct sObject* tail;
		};
	};
} Object;

typedef struct {
	//keep track of how many objects we've allocated so far
	int numObjects;
	
	//number of objects required to trigger a GC 
	int maxObjects;

	//maintain reference to first object in list of all objects 
	Object* firstObject;

	Object* stack[STACK_MAX];
	int stackSize;
} VM;

void assert(int condition, const char* message) {
	if (!condition) {
		printf("%s\n", message);
		exit(1);
	}
}

VM* newVM() {
	VM* vm = malloc(sizeof(VM));
	vm->stackSize = 0;
	vm->firstObject = NULL;
	vm->numObjects = 0;
	vm->maxObjects = INITIAL_GC_THRESHOLD;
	return vm;
}

void push(VM* vm, Object* value) {
	assert(vm->stackSize < STACK_MAX, "Stack overflow! UNSWAG");
	vm->stack[vm->stackSize++] = value;
}

Object* pop(VM* vm) {
	assert(vm->stackSize > 0, "Stack underflow! UNSWAG");
	return vm->stack[--vm->stackSize];
}

void gc(VM* vm) {
	int numObjects = vm->numObjects;
	
	markAll(vm);
	sweep(vm);

	//double threshold for garbage collection 
	vm->maxObjects = numObjects * 2;
	
	printf("Collected %d objects, %d remaining.\n", numObjects - vm->numObjects, vm->numObjects);
}

Object* newObject(VM* vm, ObjectType type) {
	//check if GC is needed before attempting to allocate any more
	if (vm->numObjects == vm->maxObjects) {
		gc(vm);
	}

	Object* object = malloc(sizeof(Object));
	object->marked = 0;
	object->type = type;
	
	//insert object into list of allocated objects 
	object->next = vm->firstObject;
	vm->firstObject = object;
	
	//increment number of objects the VM has allocated 
	vm->numObjects++;
	
	return object;
}

void pushInt(VM* vm, int intValue) {
	Object* object = newObject(vm, OBJ_INT);
	object->value = intValue;
	push(vm, object);
}

void pushFloat(VM* vm, float floatValue) {
	Object* object = newObject(vm, OBJ_FLOAT);
	object->fvalue = floatValue;
	push(vm, object);
}

void pushString(VM* vm, const char* text) {
	Object* object = newObject(vm, OBJ_STRING);
	object->text = text;
	push(vm, object);
}

Object* pushPair(VM* vm) {
	Object* object = newObject(vm, OBJ_PAIR);
	object->tail = pop(vm);
	object->head = pop(vm);
	
	push(vm, object);
	return object;
}

void mark(Object* object) {
	//if already marked, we're done. Check this first to prevent recursing on cycles of pairs that reference each other in the object graph
	if (object->marked) {
		return;
	}
	
	object->marked = 1;
	
	if (object->type == OBJ_PAIR) {
		mark(object->head);
		mark(object->tail);
	}
}

void markAll(VM* vm) {
	for (int i = 0; i < vm->stackSize; i++) {
		mark(vm->stack[i]);
	}
}

void sweep(VM* vm) {
	Object** object = &vm->firstObject;
	while (*object) {
		if (!(*object)->marked) {
			//this object was not reached, so remove it from the list and deallocate
			Object* unreached = *object;
			
			*object = unreached->next;
			free(unreached);
			
			//decrememnt number of objects the VM has allocated 
			vm->numObjects--;
		}
		else {
			//this object was succesfully reached, so unmark it for the next GC and move on to the next object
			(*object)->marked = 0;
			object = &(*object)->next;
		}
	}
}

void objectPrint(Object* object) {
	switch (object->type) {
		case OBJ_INT:
			printf("%d", object->value);
			break;
		case OBJ_FLOAT:
			printf("%f", object->fvalue);
			break;
		case OBJ_STRING:
			printf("%s", object->text);
			break;
		case OBJ_PAIR:
			printf("(");
			objectPrint(object->head);
			printf(", ");
			objectPrint(object->tail);
			printf(")");
			break;
	}
}

void freeVM(VM* vm) {
	vm->stackSize = 0;
	gc(vm);
	free(vm);
}

void test1() {
	printf("Test 1: Objects on stack are preserved.\n");
	VM* vm = newVM();
	pushInt(vm, 1);
	pushInt(vm, 2);
	
	gc(vm);
	assert(vm->numObjects == 2, "Should have preserved objects.");
	freeVM(vm);
}

void test2() {
	printf("Test 2: Unreached objects are collected.\n");
	VM* vm = newVM();
	pushInt(vm, 1);
	pushInt(vm, 2);
	pop(vm);
	pop(vm);
	
	gc(vm);
	assert(vm->numObjects == 0, "Should have collected unreached objects.");
	freeVM(vm);
}

void test3() {
	printf("Test 3: Reach nested objects.\n");
	VM* vm = newVM();
	pushInt(vm, 1);
	pushInt(vm, 2);
	pushPair(vm);
	pushInt(vm, 3);
	pushInt(vm, 4);
	pushPair(vm);
	pushPair(vm);
	
	gc(vm);
	assert(vm->numObjects == 7, "Should have reached objects.");
	freeVM(vm);
}

void test4() {
	printf("Test 4: Handle cycles.\n");
	VM* vm = newVM();
	pushInt(vm, 1);
	pushInt(vm, 2);
	Object* a = pushPair(vm);
	pushInt(vm, 3);
	pushInt(vm, 4);
	Object* b = pushPair(vm);
	
	//set up cycle, as well as make 2 and 4 unreachable.
	a->tail = b;
	b->tail = a;
	
	gc(vm);
	assert(vm->numObjects == 4, "Should have collected objects.");
	freeVM(vm);
}

void perfTest() {
	printf("Performace test.\n");
	VM* vm = newVM();
	
	for (int i = 0; i < 1000; i++) {
		for (int j = 0; j < 20; j++) {
			pushInt(vm, i);
		}
		
		for (int k = 0; k < 20; k++) {
			pop(vm);
		}
	}
	
	free(vm);
}

int main(int argc, const char * argv[]) {
	test1();
	test2();
	test3();
	test4();
	perfTest();
	
	VM* vm = newVM();
	pushString(vm, "This is a test");
	pushFloat(vm, 5.3);
	Object* pair = pushPair(vm);
	objectPrint(pair);
	
	return 0;
}