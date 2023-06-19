// /* vm.c: Generic interface for virtual memory objects. */

// #include "threads/malloc.h"
// #include "vm/vm.h"
// #include "vm/inspect.h"
// #include <hash.h>
// #include "lib/kernel/hash.h"
// #include "threads/vaddr.h"
// #include "threads/mmu.h"
// #include "threads/thread.h"
// #include "userprog/process.h"
// #include "lib/string.h"



// /* Initializes the virtual memory subsystem by invoking each subsystem's intialize codes. _ vm 초기화*/
// void
// vm_init (void) {
// 	vm_anon_init ();
// 	vm_file_init ();
// #ifdef EFILESYS  /* For project 4 */
// 	pagecache_init ();
// #endif
// 	register_inspect_intr ();
// 	/* DO NOT MODIFY UPPER LINES. */
	
// 	/* TODO: Your code goes here. */
	
// 	/* 프레임테이블 초기화*/
// }

// /* Get the type of the page. This function is useful if you want to know the
//  * type of the page after it will be initialized.
//  * This function is fully implemented now. */
// enum vm_type
// page_get_type (struct page *page) {
// 	int ty = VM_TYPE (page->operations->type);
// 	switch (ty) {
// 		case VM_UNINIT:
// 			return VM_TYPE (page->uninit.type);
// 		default:
// 			return ty;
// 	}
// }

// /* Helpers */
// static struct frame *vm_get_victim (void);
// static bool vm_do_claim_page (struct page *page);
// static struct frame *vm_evict_frame (void);

// /* Create the pending page object with initializer. If you want to create a
//  * page, do not create it directly and make it through this function or
//  * `vm_alloc_page`. */
// bool
// vm_alloc_page_with_initializer (enum vm_type type, void *upage, bool writable,
// 		vm_initializer *init, void *aux) {

// 	ASSERT (VM_TYPE(type) != VM_UNINIT)

// 	struct supplemental_page_table *spt = &thread_current ()->spt;

// 	/* Check wheter the upage is already occupied or not. 
// 	 * 이미 사용 중인지 아닌지 확인,  upage_새롭게 만들 페이지 va*/

// 	if (spt_find_page (spt, upage) == NULL) { //대응되는 페이지가 없다
// 		/* TODO: should modify the field after calling the uninit_new. */
		
// 		/*Create the page*/
// 		struct page *new_page = (struct page*)malloc(sizeof(struct page));

// 		/*fetch the initialier according to the VM type
// 		 * and then create "uninit" page struct by calling uninit_new.*/
// 		if(new_page == NULL)
// 			return false;
// 		switch(VM_TYPE(type)){	
// 			case VM_ANON: //익명 페이지
// 				uninit_new(new_page, upage, init, type, aux,anon_initializer);
// 				break;
// 			case VM_FILE: //파일과 관련된 페이지
// 				uninit_new(new_page, upage, init, type, aux,file_backed_initializer);
// 				break;
// 			default:
// 				break;
// 		}
// 		new_page->writable = writable;
// 		/* TODO: insert the page into the spt. */
// 		return spt_insert_page(spt, new_page);
// 	}
// err:
// 	return false;
// }

// /* Find VA from spt and return page. On error, return NULL.
//  * 인자로 넘겨진 페이지테이블에서 가상 주소와 대응되는 페이지 구조체를 찾아서 반환, 수정 */
// struct page *
// spt_find_page (struct supplemental_page_table *spt /*UNUSED*/, void *va /*UNUSED*/) {
// 	ASSERT(va!= NULL);

// 	struct page p;
// 	struct hash_elem *e;
// 	p.va = pg_round_down(va); //va가 중간값으로 들어왔을때 entry값에 맞추기위해
// 	e = hash_find(&spt->spt_hash, &p.h_elem); 
// 	if(e == NULL){
// 		return NULL;
// 	}
// 	return hash_entry(e, struct page, h_elem);
	
// }


// /* Insert PAGE into spt with validation.
//  * 인자(보조 페이지 테이블)에 페이지 구조체를 삽입
//  * 주어진 테이블에 가상 주소 존재 유무 검사를 해야함*/
// bool
// spt_insert_page (struct supplemental_page_table *spt /*UNUSED*/,struct page *page /*UNUSED*/){
// 	struct hash_elem* ins_hash = hash_insert(&spt->spt_hash, &page->h_elem);
// 	if(ins_hash == NULL){
// 		return true;
// 	}
// 	return false;
// 	// if(spt_find_page(spt,page->va) == NULL){ /*수정, 이미 있는 페이지가 아니라면, 테이블 만들어라*/
// 	// 	hash_insert(spt->spt_hash,&page->h_elem);
// 	// 	succ = true;
// 	// }
// 	// return succ;
// }


// void //TODO: 구현해야함?
// spt_remove_page (struct supplemental_page_table *spt, struct page *page) {
// 	vm_dealloc_page (page);
	
// 	struct hash_elem *del_hash = hash_delete(&spt->spt_hash, &page->h_elem);
// 	if(del_hash != NULL){
// 		return true;
// 	}
// 	else{
// 		return false;
// 	}
// 	return true;
// }

// /* Get the struct frame, that will be evicted. 
//  * 	받아온 프레임을 swap-out해주는 함수*/
// static struct frame *
// vm_get_victim (void) {
// 	struct frame *victim = NULL;
// 	/*TODO: the policy for eviction is up to you*/
// 	// swap_out(victim->page);
// 	return victim;
// }

// /* Evict one page and return the corresponding frame.
//  * Return NULL on error.*/
// static struct frame *
// vm_evict_frame (void) {
// 	struct frame *victim UNUSED = vm_get_victim ();
// 	/* TODO: swap out the victim and return the evicted frame. */

// 	return NULL;
// }

// /* palloc() and get frame. If there is no available page, evict the page and return it. This always return valid address
//  * palloc을 호출, 프레임 가져옴 / 사용가능 페이지x, 페이지 재거, 반환 *************	수정
//  * That is, if the user pool memory is full, this function evicts the frame to get the available memory space.*/
// static struct frame *
// vm_get_frame (void) {
// 	struct frame *frame = malloc(sizeof(struct frame));
// 	frame->kva = palloc_get_page(PAL_USER);
// 	if(frame->kva == NULL){
// 		// frame->kva = vm_evict_frame();
// 		PANIC("todo");
// 	}else{
// 		frame->page = NULL;
// 		ASSERT (frame != NULL);
// 		ASSERT (frame->page == NULL);
// 		return frame;
// 	}
	
// }

// /* Growing the stack. */
// static void
// vm_stack_growth (void *addr UNUSED) {
// }

// /* Handle the fault on write_protected page */
// static bool
// vm_handle_wp (struct page *page UNUSED) {
// }

// /* Return true on success */
// /*인터럽트 프레임 포인터, 페이지 폴트 발생주소, 사용자 모드 발생여부, 쓰기 작업중 발생 여부, 페이지 존재하지 않아서 발생한지 여부*/
// bool
// vm_try_handle_fault (struct intr_frame *f UNUSED, void *addr UNUSED, bool user UNUSED, bool write UNUSED, bool not_present UNUSED) {
// 	struct supplemental_page_table *spt UNUSED = &thread_current ()->spt;
// 	struct page *page = NULL;
// 	page = spt_find_page(spt,addr);
// 	/* TODO: Validate the fault */
// 	/* TODO: Your code goes here */ 
// 	/* f, addr이 null이 아닌지 확인
// 	 * addr을 검사해 해당 주소가 유효한지확인
// 	 * 유효하면 해당 페이지 폴트 처리 -> 페이지 할당 or swap영역에서 데이터를 읽엉오는 등의 작업수행
// 	 * 성공시 true 반환*/
// 	if((is_kernel_vaddr(addr) == NULL) || addr == NULL){
// 		return false;
// 	}
// 	if(page== NULL){
// 		return false;
// 	}
// 	return vm_do_claim_page (page); 
// }

// /* Free the page.
//  * DO NOT MODIFY THIS FUNCTION. */
// void
// vm_dealloc_page (struct page *page) {
// 	destroy (page);
// 	free (page);
// }

// /* Claim the page that allocate on VA. va와 연결할 페이지를 spt에서 찾음*/
// bool
// vm_claim_page (void *va UNUSED) {
// 	struct page *page = NULL;
// 	page = spt_find_page(&thread_current()->spt,va);  // struct page *page  = spt_find_page(&thread_current ()->spt, va);==> 함수결과가 유효하지 못하면 page가 제대로 정의되지 않는다
// 	if(page == NULL){
// 		return false;
// 	}

// 	return vm_do_claim_page (page); /*연결하고자하는 페이지*/
// }

// /* Claim the PAGE and set up the mmu. 
//  * 페이지가 이미 연결되어있는지 확인하기*/
// static bool
// vm_do_claim_page (struct page *page) {
// 	struct frame *frame = vm_get_frame();
// 		/* Set links */
// 	if(frame == NULL){
// 		frame->page = page;
// 		page->frame = frame;
// 	}

// 	if(install_page(page->va, frame->kva, page->writable)){
// 		return swap_in(page, frame->kva);
// 	}

// 	// if(pml4_get_page(thread_current()->pml4, page->va) == NULL && pml4_set_page(thread_current()->pml4, page->va, frame->kva, page->writable)){
// 		// return swap_in(page, frame->kva); /* va를 pa에 매핑하기 위해 페이지 테이블 항목에 삽입한다 */
// 	 //물리적 주소가 비어있고, 
// 	 	return false;
// }
// bool
// vm_destroy_func(struct hash_elem *h_e, void *aux){
// 	/*hash_elem가져오기*/
// 	struct page *p = hash_entry(h_e, struct page, h_elem);
// 	if(p->is_loaded){
// 		vm_dealloc_page(p);
// 	}
// }

// /* Returns a hash value for page p. */
// unsigned
// page_hash (const struct hash_elem *p_, void *aux) {
// 	const struct page *p = hash_entry (p_, struct page, h_elem);
// 	return hash_bytes (&p->va, sizeof p->va); //byte랑 Int 차일가...
// }

// /* Returns true if page a precedes page b. */
// bool
// page_less (const struct hash_elem *a_, const struct hash_elem *b_, void *aux) {
// 	const struct page *a = hash_entry (a_, struct page, h_elem);
// 	const struct page *b = hash_entry (b_, struct page, h_elem);
// 	return (a->va < b->va);
// }


// /* Initialize new supplemental page table
//  * 보조페이지 테이블 초기화
//  * initd함수로 새로운 프로세스 시작, do_fork로 자식 프로세스 생성될 때 호출*/
// void
// supplemental_page_table_init (struct supplemental_page_table *spt UNUSED) {
// 	hash_init(&spt->spt_hash,page_hash,page_less,NULL);
// 	return;
// }

// /* Copy supplemental page table from src to dst */
// /*부모 프로세스가 가지고있는 spt정보를 자식 프로세스에게 복사*/

// bool
// supplemental_page_table_copy (struct supplemental_page_table *dst /*UNUSED*/, struct supplemental_page_table *src /*UNUSED*/) {
// 	struct hash_iterator i;
	
// 	/*테이블 순회*/
// 	hash_first (&i, &src->spt_hash);

// 	while (hash_next (&i)) {
// 		struct page *p = hash_entry(hash_cur (&i), struct page, h_elem);
// 		struct page *child_p = spt_find_page(dst, p->va);

// 		if(!vm_alloc_page_with_initializer(p->uninit.type,p->va, p->writable, p->uninit.init,p->uninit.aux)){
// 			return false;
// 		}
// 		if(p->frame){ //vm_claim_page를 do로 수정
// 			if(!vm_do_claim_page(child_p)){
// 				return false;
// 			}
// 			memcpy(child_p->frame->kva,p->frame->kva,PGSIZE);
// 		}
// 		return true;
// 	}
// 	// while()
// 	/*spt를 interation 해야함
// 	iter돌때마다 해당 hash_elem과 연결된 page를 찾아서 해당 구조체의 정보를 저장
// 	vm_alloc_page_with_initializer함수 인자를 참고
// 	부모 페이지들의 정보를 저장한 뒤, 자식이 가질 새로운 페이지를 생성해야함 부모 페이지 타입확인해야함
// 	uninit 페이지가 아닌경우 set_stack처럼 페이지를 생성한 뒤 탑입에 맞는 ㅑnitializer을 호출해 페이지 타입을 변경시킴
// 	and thaen 부모 페이지의 물리 메모리 정보를 자식에게도 복사
// 	성공하면 return true*/

// }

// /* Free the resource hold by the supplemental page table 
//  * 해시 테이블 제거 */
// void
// supplemental_page_table_kill (struct supplemental_page_table *spt UNUSED) {
// 	/* TODO: Destroy all the supplemental_page_table hold by thread and
// 	 * TODO: writeback all the modified contents to the storage. */
// 	hash_destroy(&spt, vm_destroy_func);
	
// }
// // struct list frame_table
/* vm.c: Generic interface for virtual memory objects. */

#include "threads/malloc.h"
#include "vm/vm.h"
#include "vm/inspect.h"
#include "threads/vaddr.h"
#include "threads/mmu.h"
#include "userprog/process.h"
#include "lib/string.h"

/* Initializes the virtual memory subsystem by invoking each subsystem's
 * intialize codes. */
void
vm_init (void) {
	vm_anon_init ();
	vm_file_init ();
#ifdef EFILESYS  /* For project 4 */
	pagecache_init ();
#endif
	register_inspect_intr ();
	/* DO NOT MODIFY UPPER LINES. */
	/* TODO: Your code goes here. */
}

/* Get the type of the page. This function is useful if you want to know the
 * type of the page after it will be initialized.
 * This function is fully implemented now. */
enum vm_type
page_get_type (struct page *page) {
	int ty = VM_TYPE (page->operations->type);
	switch (ty) {
		case VM_UNINIT:
			return VM_TYPE (page->uninit.type);
		default:
			return ty;
	}
}

/* Helpers */
static struct frame *vm_get_victim (void);
static bool vm_do_claim_page (struct page *page);
static struct frame *vm_evict_frame (void);

/* Create the pending page object with initializer. If you want to create a
 * page, do not create it directly and make it through this function or
 * `vm_alloc_page`. */
bool
vm_alloc_page_with_initializer (enum vm_type type, void *upage, bool writable,
		vm_initializer *init, void *aux) {

	ASSERT (VM_TYPE(type) != VM_UNINIT)

	struct supplemental_page_table *spt = &thread_current ()->spt;

	/* Check wheter the upage is already occupied or not. */
	if (spt_find_page (spt, upage) == NULL) {
		/* TODO: Create the page, fetch the initialier according to the VM type,
		 * TODO: and then create "uninit" page struct by calling uninit_new. You
		 * TODO: should modify the field after calling the uninit_new. */

		// 1) 페이지 생성
		struct page* new_page = (struct page*)malloc(sizeof(struct page));
		if(new_page==NULL) return false;
		// 2) type에 따라 적절한 initializer fetch
		// 3) uninit_new로 uninit 페이지 생성
		switch (VM_TYPE(type))
		{
			case VM_ANON:
				uninit_new(new_page, upage, init, type, aux, anon_initializer);
				break;
			case VM_FILE:
				uninit_new(new_page, upage, init, type, aux, file_backed_initializer);
				break;
			default:
				break;
		}
		// 4) 그 후 field 수정
		new_page->writable = writable;

		/* TODO: Insert the page into the spt. */
		// spt에 페이지 삽입
		return spt_insert_page(spt,new_page);
	}
err:
	return false;
}

/* Find VA from spt and return page. On error, return NULL. */
struct page *
spt_find_page (struct supplemental_page_table *spt /*UNUSED*/, void *va /*UNUSED*/) {
	// struct page *page = NULL;
	/* TODO: Fill this function.
	 * 인자로 넘겨진 보조 페이지 테이블에서 가상주소 va와 대응되는 페이지 구조체를 
	 * 찾아 반환 (실패 시 NULL 반환) */
	ASSERT(va!=NULL)
	struct page tmp_page;
	tmp_page.va = pg_round_down(va);
	struct hash_elem *h = hash_find(&spt->table,&tmp_page.h_elem);
	if(h==NULL) return NULL;
	return hash_entry(h, struct page, h_elem);
}

/* Insert PAGE into spt with validation. */
bool
spt_insert_page (struct supplemental_page_table *spt /*UNUSED*/,
		struct page *page /*UNUSED*/) {
	int succ = false;
	/* TODO: Fill this function. */
	/* 인자로 주어진 보조 페이지 테이블에 페이지 구조체 삽입
	 * 주어진 보조 테이블에 해당 가상주소의 존재 여부를 확인하고 진행*/
	// TODO: set vm entry
	struct hash_elem* ins_hash = hash_insert(&spt->table, &page->h_elem);
	if(ins_hash==NULL) return true;
	return false;
}

void
spt_remove_page (struct supplemental_page_table *spt, struct page *page) {
	vm_dealloc_page (page);
	/*hash_delete() 함수를 이용하여 vm_entry를 해시 테이블에서 제거*/
	struct hash_elem* del_hash = hash_delete(&spt->table,&page->h_elem);
	if (del_hash != NULL) return true;
	else return false;
	return true;
}

/* Get the struct frame, that will be evicted. */
static struct frame *
vm_get_victim (void) {
	struct frame *victim = NULL;
	 /* TODO: The policy for eviction is up to you. */

	return victim;
}

/* Evict one page and return the corresponding frame.
 * Return NULL on error.*/
static struct frame *
vm_evict_frame (void) {
	struct frame *victim UNUSED = vm_get_victim ();
	/* TODO: swap out the victim and return the evicted frame. */

	return NULL;
}

/* palloc() and get frame. If there is no available page, evict the page
 * and return it. This always return valid address. That is, if the user pool
 * memory is full, this function evicts the frame to get the available memory
 * space.*/
static struct frame *
vm_get_frame (void) {
	struct frame *frame = malloc(sizeof(struct frame));
	/* TODO: Fill this function. */
	/* palloc_get_page 호출하여 메모리 풀에서 새로운 물리메모리 페이지를 가져옴
	 *	- 성공 시 프레임 할당, 프레임 구조체 초기화 후 프레임 반환
	 *	- TODO_LATER: 실패 시 swap out(나중에 하기)
	 * 	- 구현 이후, 모든 유저 공간 페이지들을 이 함수로 할당
	 */
	frame->kva = palloc_get_page(PAL_USER);
	if (frame->kva != NULL){
		frame->page=NULL;
		ASSERT (frame != NULL);
		ASSERT (frame->page == NULL);
		return frame;
	}
	else {
		// free(frame);
		PANIC("todo");
	 }
}

/* Growing the stack. */
static void
vm_stack_growth (void *addr UNUSED) {
}

/* Handle the fault on write_protected page */
static bool
vm_handle_wp (struct page *page UNUSED) {
}

/* Return true on success */
bool
vm_try_handle_fault (struct intr_frame *f UNUSED, void *addr UNUSED,
		bool user UNUSED, bool write UNUSED, bool not_present UNUSED) {
			
	struct supplemental_page_table *spt UNUSED = &thread_current ()->spt;
	struct page *page = NULL;
	/* TODO: Validate the fault */
	/* TODO: Your code goes here */
	/*spt_find_page 를 거쳐 보조 페이지 테이블을 참고하여
		fault된 주소에 대응하는 페이지 구조체를 해결하기 위한 함수*/
	if(is_kernel_vaddr(addr)||addr==NULL) {
		return false;
	}
	page = spt_find_page(spt, addr);
	if(page==NULL) return false;
	return vm_do_claim_page (page);
}

/* Free the page.     	
 * DO NOT MODIFY THIS FUNCTION. */
void
vm_dealloc_page (struct page *page) {
	destroy (page);
	free (page);
}

/* Claim the page that allocate on VA. */
bool
vm_claim_page (void *va UNUSED) {
	struct page *page = NULL;
	/* TODO: Fill this function */
	/* 인자로 주어진 va page에 페이지 할당
	 * 페이지 하나를 얻고, vm_do_claim_page가 호출되어야 한다.	 */
	page = spt_find_page(&thread_current()->spt, va);
	if (page==NULL) return false;
	return vm_do_claim_page (page);
}

/* Claim the PAGE and set up the mmu. */
/* 페이지에 물리 메모리 프레임 할당 후 mmu 세팅*/
static bool
vm_do_claim_page (struct page *page) {
	/* vm_get_frame으로 프레임을 얻은 후*/
	struct frame *frame = vm_get_frame ();

	/* Set links */
	if (frame == NULL)
	{
		return false;
	}
	frame->page = page;
	page->frame = frame;

	/* MMU 세팅 */
	/* TODO: Insert page table entry to map page's VA to frame's PA.*/
	/* 가상 주소와 물리 주소를 매핑한 정보를 페이지 테이블에 추가
	 * 	- 성공 여부 true/false로 반환															 */
	// if (pml4_get_page(thread_current()->pml4, page->va) == NULL && pml4_set_page(thread_current()->pml4, page->va, frame->kva, page->writable))
	if(install_page(page->va,frame->kva,page->writable))
	return swap_in (page, frame->kva);

	return false;
}

//uint64_t hash_hash_func (const struct hash_elem *e, void *aux);
uint64_t spt_hash_func(const struct hash_elem *e, void *aux)
{
	struct page* p = hash_entry(e, struct page, h_elem);
	return hash_int(p->va);
}

bool spt_less_func(const struct hash_elem *a, const struct hash_elem *b, void *aux)
{
	struct page* a_entry = hash_entry(a, struct page, h_elem);
	struct page* b_entry = hash_entry(b, struct page, h_elem);
	return (a_entry->va < b_entry->va);
}

/* Initialize new supplemental page table */
void
supplemental_page_table_init (struct supplemental_page_table *spt UNUSED) {
	/* TODO: 보조 페이지 테이블 초기화(구조는 스스로 결정)
	 * initd나 __do_fork할 때 호출
	 */
	hash_init(&spt->table,spt_hash_func,spt_less_func,NULL);
	return;
}

/* Copy supplemental page table from src to dst */
bool
supplemental_page_table_copy (struct supplemental_page_table *dst /*UNUSED*/,
		struct supplemental_page_table *src /*UNUSED*/) {
	
	struct hash_iterator i;
	hash_first (&i, &src->table);

	while (hash_next (&i)) {
		struct page *p = hash_entry (hash_cur (&i), struct page, h_elem);
		// struct page* new_page = (struct page*)&page_arr[idx++];
		// if (page_get_type(p)!=VM_UNINIT) {
		// 	if(!vm_alloc_page(VM_TYPE(page_get_type(p)),p->va,p->writable)) return false;;
		// 	if(!vm_claim_page(p->va)) return false;
		// 	struct page* dst_page = spt_find_page(dst,p->va);
		// 	if(dst_page) memcpy(dst_page->frame->kva,p->frame->kva,PGSIZE);
		if(!vm_alloc_page_with_initializer(p->uninit.type,p->va,p->writable,p->uninit.init,p->uninit.aux))
			return false;
		struct page *child_p = spt_find_page(dst,p->va);
		if (p->frame){
			if(!vm_do_claim_page(child_p)) return false;
			memcpy(child_p->frame->kva, p->frame->kva,PGSIZE);
		}
	}

	return true;
}


// typedef void hash_action_func (struct hash_elem *e, void *aux);
void
vm_destroy_func(struct hash_elem *e, void* aux)
{
	/* Get hash element (hash_entry() 사용) */
	struct page* p = hash_entry(e, struct page, h_elem);
	/* TODO: load가 되어 있는 page의 vm_entry인 경우
	page의 할당 해제 및 page mapping 해제 (palloc_free_page()와
	pagedir_clear_page() 사용) */
	if (p->is_loaded){
		// palloc_free_page(p->va);
		// pml4_clear_page(thread_current()->pml4,p);
		vm_dealloc_page(p);
	}
	/* TODO:vm_entry 객체 할당 해제 */
	// free(p);
}

/* Free the resource hold by the supplemental page table */
/* 해시 테이블 제거 함수 */
void
supplemental_page_table_kill (struct supplemental_page_table *spt UNUSED) {
	/* TODO: Destroy all the supplemental_page_table hold by thread and
	 * TODO: writeback all the modified contents to the storage. */
	
	hash_destroy(&spt->table, vm_destroy_func);
}
