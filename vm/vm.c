/* vm.c: Generic interface for virtual memory objects. */

#include "threads/malloc.h"
#include "vm/vm.h"
#include "vm/inspect.h"
#include <hash.h>
#include "lib/kernel/hash.h"
#include "threads/vaddr.h"
#include "threads/mmu.h"
#include "threads/thread.h"

/* Initializes the virtual memory subsystem by invoking each subsystem's intialize codes. _ vm 초기화*/
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
/* 초기화로 보류 중인 페이지 객체를 생성, 페이지를 생성하려면 vc_alloc_page를 통해 만들어야함*/
bool
vm_alloc_page_with_initializer (enum vm_type type, void *upage, bool writable,
		vm_initializer *init, void *aux) {

	ASSERT (VM_TYPE(type) != VM_UNINIT)

	struct supplemental_page_table *spt = &thread_current ()->spt;

	/* Check wheter the upage is already occupied or not. 
	 * 이미 사용 중인지 아닌지 확인*/
	if (spt_find_page (spt, upage) == NULL) { //대응되는 페이지가 없다
		/* TODO: Create the page, fetch the initialier according to the VM type 페이지 만들기, 일치하는 가상메모리를 가져온다,
		 * TODO: and then create "uninit" page struct by calling uninit_new. You
		 * TODO: should modify the field after calling the uninit_new. */
		//vm_alloc_page로 만들어야할거같은데..
		/* TODO: I      
		+nsert the page into the spt. */
		spt_insert_page(spt, upage); /*추가*/ 
		
	}
err:
	return false;
}

/* Find VA from spt and return page. On error, return NULL.
 * 인자로 넘겨진 페이지테이블에서 가상 주소와 대응되는 페이지 구조체를 찾아서 반환, 수정 */
struct page *
spt_find_page (struct supplemental_page_table *spt UNUSED, void *va UNUSED) {
	struct page p;
	struct hash_elem *e;
	p.va = pg_round_down(va); //va가 중간값으로 들어왔을때 entry값에 맞추기위해
	e = hash_find(spt->spt_hash, &p.h_elem);
	if(e == NULL){
		return NULL;
	return hash_entry(e, struct page, h_elem);
	}
}


/* Insert PAGE into spt with validation.
 * 인자(보조 페이지 테이블)에 페이지 구조체를 삽입
 * 주어진 테이블에 가상 주소 존재 유무 검사를 해야함
 * */
bool
spt_insert_page (struct supplemental_page_table *spt UNUSED,struct page *page UNUSED){
	struct hash_elem* ins_hash = hash_insert(&spt->spt_hash, &page->h_elem);
	if(ins_hash == NULL){
		return true;
	}
	return false;
	// if(spt_find_page(spt,page->va) == NULL){ /*수정, 이미 있는 페이지가 아니라면, 테이블 만들어라*/
	// 	hash_insert(spt->spt_hash,&page->h_elem);
	// 	succ = true;
	// }
	// return succ;
}


void //TODO: 구현해야함?
spt_remove_page (struct supplemental_page_table *spt, struct page *page) {
	vm_dealloc_page (page);
	return true;
}

/* Get the struct frame, that will be evicted. 
 * 	받아온 프레임을 swap-out해주는 함수*/
static struct frame *
vm_get_victim (void) {
	struct frame *victim = NULL;
	/*TODO: the policy for eviction is up to you*/
	swap_out(victim->page);
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

/* palloc() and get frame. If there is no available page, evict the page and return it. This always return valid address
 * palloc을 호출, 프레임 가져옴 / 사용가능 페이지x, 페이지 재거, 반환 *************	수정
 * That is, if the user pool memory is full, this function evicts the frame to get the available memory space.*/
static struct frame *
vm_get_frame (void) {
	struct frame *frame = malloc(sizeof(struct frame));
	frame->kva = palloc_get_page(PAL_USER);
	if(frame == NULL){
		// frame->kva = vm_evict_frame();
		PANIC("todo");
	}
	frame->page = NULL;

	ASSERT (frame != NULL);
	ASSERT (frame->page == NULL);
	return frame;
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

	return vm_do_claim_page (page); 
}

/* Free the page.
 * DO NOT MODIFY THIS FUNCTION. */
void
vm_dealloc_page (struct page *page) {
	destroy (page);
	free (page);
}

/* Claim the page that allocate on VA. va와 연결할 페이지를 spt에서 찾음 *******수정 */
bool
vm_claim_page (void *va UNUSED) {
	struct page *page = NULL;
	page = spt_find_page(&thread_current()->spt,va);  // struct page *page  = spt_find_page(&thread_current ()->spt, va);==> 함수결과가 유효하지 못하면 page가 제대로 정의되지 않는다
	if(page == NULL){
		return false;
	}

	return vm_do_claim_page (page); /*연결하고자하는 페이지*/
}

/* Claim the PAGE and set up the mmu. 
 * 페이지가 이미 연결되어있는지 확인하기*/
static bool
vm_do_claim_page (struct page *page) {
	struct frame *frame = vm_get_frame();
		/* Set links */
	frame->page = page;
	page->frame = frame;

	if(pml4_get_page(thread_current()->plm4, page->va) == NULL && pml4_set_page(thread_current()->pml4, page->va, frame->kva, page->writable)){
		return swap_in(page, frame->kva); /* va를 pa에 매핑하기 위해 페이지 테이블 항목에 삽입한다 */
	} //물리적 주소가 비어있고, 
	 

	return false;
}

/* Returns a hash value for page p. */
unsigned
page_hash (const struct hash_elem *p_, void *aux) {
	const struct page *p = hash_entry (p_, struct page, h_elem);
  return hash_bytes (&p->va, sizeof p->va); //byte랑 Int 차일가...
}

/* Returns true if page a precedes page b. */
bool
page_less (const struct hash_elem *a_, const struct hash_elem *b_, void *aux) {
  const struct page *a = hash_entry (a_, struct page, h_elem);
  const struct page *b = hash_entry (b_, struct page, h_elem);
  return (a->va < b->va);
}


/* Initialize new supplemental page table
 * 보조페이지 테이블 초기화
 * initd함수로 새로운 프로세스 시작, do_fork로 자식 프로세스 생성될 때 호출*/
void
supplemental_page_table_init (struct supplemental_page_table *spt UNUSED) {
	hash_init(&spt->spt_hash,page_hash,page_less,NULL);
}

/* Copy supplemental page table from src to dst */
bool
supplemental_page_table_copy (struct supplemental_page_table *dst UNUSED,
		struct supplemental_page_table *src UNUSED) {
}

/* Free the resource hold by the supplemental page table 
 * 해시 테이블 제거 */
void
supplemental_page_table_kill (struct supplemental_page_table *spt UNUSED) {
	/* TODO: Destroy all the supplemental_page_table hold by thread and
	 * TODO: writeback all the modified contents to the storage. */
}
// struct list frame_table
