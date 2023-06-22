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
/* 완료*/
bool
vm_alloc_page_with_initializer (enum vm_type type, void *upage, bool writable,
		vm_initializer *init, void *aux) {

	ASSERT (VM_TYPE(type) != VM_UNINIT)

	struct supplemental_page_table *spt = &thread_current ()->spt;

	/* Check wheter the upage is already occupied or not. */
	if (spt_find_page (spt, upage) == NULL) {
		struct page *new = (struct page*)malloc(sizeof(struct page));
		/* TODO: Create the page, fetch the initialier according to the VM type,
		 * TODO: and then create "uninit" page struct by calling uninit_new. You
		 * TODO: should modify the field after calling the uninit_new. */

		// type에 따라 적절한 initializer fetch
		// uninit_new로 uninit 페이지 생성
		if(new == NULL) return false;
		switch(VM_TYPE(type)){
			case VM_FILE :
				uninit_new(new, upage, init, type, aux, file_backed_initializer);
				break;
			case VM_ANON :
				uninit_new(new, upage, init, type, aux, anon_initializer);
				break;
			default:
				break;
		}
		/* TODO: Insert the page into the spt. */
		/*필드 수정*/
		new->writable = writable;
		/*spt에 페이지 삽입*/
		return spt_insert_page(spt,new);
	}

err:
	return false;
}

/* Find VA from spt and return page. On error, return NULL. */
/*완료*/
struct page *
spt_find_page (struct supplemental_page_table *spt /*UNUSED*/, void *va /*UNUSED*/) {
	// struct page *page = NULL;
	/* TODO: Fill this function. */
	ASSERT (va!=NULL)

	struct page p;
	p.va = pg_round_down(va);
	struct hash_elem *e = hash_find(&spt->table, &p.h_elem);

	if(e == NULL) return NULL;
	else return hash_entry(e, struct page, h_elem);

}

/* Insert PAGE into spt with validation. */
/* 완료*/
bool
spt_insert_page (struct supplemental_page_table *spt /*UNUSED*/,
		struct page *page /*UNUSED*/) {
	int succ = false;
	/* TODO: Fill this function. */
	/*가상주소 존재하는지 확인*/
	struct hash_elem *h = hash_insert(&spt->table,&page->h_elem);
	if(h == NULL) return true;
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
/* 완료 */
static struct frame *
vm_get_frame (void) {
	struct frame *frame = malloc(sizeof(struct frame));
	/* TODO: Fill this function. */
	/* 메모리 풀에서 새로운 물리메모리 페이지를 가져옴*/

	frame->kva= palloc_get_page(PAL_USER);
	/*할당받지 못했으면*/
	if(frame->kva == NULL){
		PANIC("todo");
	} /*프레임 할당, 프레임 구조체 초기화 후 프레임 반환*/
	else{
		frame->page = NULL;
		ASSERT (frame != NULL);
		ASSERT (frame->page == NULL);
		return frame;
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
/* 물리 프레임과 연결할 페이지를 spt를 통해 찾아주고 do_claim 호출*/
/* 완료*/
bool
vm_claim_page (void *va UNUSED) {
	struct page *page = NULL;
	/* TODO: Fill this function */
	page = spt_find_page(&thread_current()->spt, va);
	if(page == NULL) return false;

	return vm_do_claim_page (page);
}

/* Claim the PAGE and set up the mmu. */
/* 완료*/
static bool
vm_do_claim_page (struct page *page) {
	struct frame *frame = vm_get_frame ();
	/*프레임이 없을 경우*/
	if(frame == NULL) return false;
	/* Set links */
	frame->page = page;
	page->frame = frame;

	/* MMU 세팅 */
	/* TODO: Insert page table entry to map page's VA to frame's PA.*/
	/* 가상 주소와 물리 주소를 매핑한 정보를 페이지 테이블에 추가
	 * 	- 성공 여부 true/false로 반환*/

	if(install_page(page->va,frame->kva,page->writable))
		return swap_in (page, frame->kva);
	return false;
}
/* 다름*/
unsigned
page_hash (const struct hash_elem *p_, void *aux UNUSED) {
  const struct page *p = hash_entry (p_, struct page, h_elem);
  return hash_bytes (&p->va, sizeof p->va);
}
/* Returns true if page a precedes page b. */
// uint64_t spt_hash_func(const struct hash_elem *e, void *aux)
// {
// 	struct page* p = hash_entry(e, struct page, h_elem);
// 	return hash_int(p->va);
// }
bool
page_less(const struct hash_elem *a_,
           const struct hash_elem *b_, void *aux UNUSED) {
  const struct page *a = hash_entry (a_, struct page, h_elem);
  const struct page *b = hash_entry (b_, struct page, h_elem);

  return a->va < b->va;
}

/* Initialize new supplemental page table */
void
supplemental_page_table_init (struct supplemental_page_table *spt UNUSED) {
	/* TODO: 보조 페이지 테이블 초기화(구조는 스스로 결정)
	 * initd나 __do_fork할 때 호출*/
	hash_init(&spt->table,page_hash,page_less,NULL);
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

void
vm_destroy_func(struct hash_elem *e, void* aux)
{

	/* Get hash element (hash_entry() 사용) */
	struct page* p = hash_entry(e, struct page, h_elem);
	/* TODO: load가 되어 있는 page의 vm_entry인 경우
	   page의 할당 해제 및 page mapping 해제 
	   (palloc_free_page()와 pagedir_clear_page() 사용) */

	if (p->is_loaded){
		// palloc_free_page(p->va);
		// pml4_clear_page(thread_current()->pml4,p);
		vm_dealloc_page(p);
	}
	/* TODO:vm_entry 객체 할당 해제 */
	// free(p);
}

/* Free the resource hold by the supplemental page table */
void
supplemental_page_table_kill (struct supplemental_page_table *spt UNUSED) {
	/* TODO: Destroy all the supplemental_page_table hold by thread and
	 * TODO: writeback all the modified contents to the storage. */
	
	hash_destroy(&spt->table, vm_destroy_func);
	}
