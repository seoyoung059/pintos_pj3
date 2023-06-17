#ifndef VM_VM_H
#define VM_VM_H
#include <stdbool.h>
#include "threads/palloc.h"
#include "lib/kernel/hash.h"
#include"lib/kernel/list.h"

enum vm_type {
	/* page not initialized */
	VM_UNINIT = 0,
	/* page not related to the file, aka anonymous page */
	VM_ANON = 1,
	/* page that realated to the file */
	VM_FILE = 2,
	/* page that hold the page cache, for project 4 */
	VM_PAGE_CACHE = 3,

	/* Bit flags to store state */

	/* Auxillary bit flag marker for store information. You can add more
	 * markers, until the value is fit in the int. */
	VM_MARKER_0 = (1 << 3),
	VM_MARKER_1 = (1 << 4),

	/* DO NOT EXCEED THIS VALUE. */
	VM_MARKER_END = (1 << 31),
};

#include "vm/uninit.h"
#include "vm/anon.h"
#include "vm/file.h"
#ifdef EFILESYS
#include "filesys/page_cache.h"
#endif

struct page_operations;
struct thread;

#define VM_TYPE(type) ((type) & 7)

/* The representation of "page".
 * This is kind of "parent class", which has four "child class"es, which are
 * uninit_page, file_page, anon_page, and page cache (project4).
 * DO NOT REMOVE/MODIFY PREDEFINED MEMBER OF THIS STRUCTURE. */
 
struct page /*가상메모리에서 페지를 의미하는 구조체_vm_entry구조체 추가해야지 왜??*/
	{ 
	const struct page_operations *operations;
	void *va;              /* Address in terms of user space, virtual memory */
	struct frame *frame;   /* Back reference for frame */
	
	/************************추가**********************/
	bool writable; /*true -> 해당 주소 write가능, false 불가능*/
	
	bool is_loaded; /*물리메모리의 탑재 여부를 알려주는 플래그*/
	struct file* file; /*가상주소랑 매핑된 파일*/

	struct list_elem mmap_elem; /*mmap리스트 element*/

	size_t offsetof; /*읽어야 할 파일 오프셋*/
	size_t read_bytes; /*가상페이지에 쓰여있는 데이터 크기*/
	size_t zero_bytes; /*0으로 채울 남은 페이지 바이트*/

	size_t swap_slot; /*스왑 슬롯*/

	struct hash_elem h_elem; /*key = pafe->va, value = va에 매핑되는 struct page*/


	/* Per-type data are binded into the union.
	 * Each function automatically detects the current union */
	/*하나의 메모리 영역에 다른 타입의 데이터를 저장하는 것을 허용하는 자료형 = Sturct처럼 각 원소들이 다른 자료형을 가짐
	 *union  크기가 최대인 멤버의 크기만큼 할당받아 멤버들이 그 메모리를 공유 = 한 번에 한 멤버가 공간 사용가능*/
	union 
{
		struct uninit_page uninit;
		struct anon_page anon;
		struct file_page file;
		/*페이지는 세 가지 종류 중 하나
		 *swap in, swap out, 페이지 삭제와 같은 동작을 수행*/
#ifdef EFILESYS
		struct page_cache page_cache;
#endif
	};
};


/* The representation of "frame" _ 필요에 따라 멤버추가해도된다*/
struct frame {
	void *kva; /*커널 가상 주소*/
	struct page *page; /*페이지 구조체 담기 위한 멤버변수*/

	struct list_elem frame_elem; /*추가, frame구조체 추가*/
};

/* The function table for page operations.
 * This is one way of implementing "interface" in C.
 * Put the table of "method" into the struct's member, and
 * call it whenever you needed. */
/*구조체를 3개의 함수 포인터를 포함한 하나의 함수 테이블
 *함수 혹은 메모리에 있는 실행가능한 코드를 가리키는 포인터
 *destroy(page)라는 함수 호출로 충분히 가능*/
struct page_operations {
	bool (*swap_in) (struct page *, void *); /*스왑인 = 보조 저장소 -> ram으로 페이지 전송*/
	bool (*swap_out) (struct page *); /*스왑아웃 = ram -> 보조정장소*/
	void (*destroy) (struct page *); /*페이지 파괴*/
	enum vm_type type;
};

#define swap_in(page, v) (page)->operations->swap_in ((page), v)
#define swap_out(page) (page)->operations->swap_out (page)
#define destroy(page) \
	if ((page)->operations->destroy) (page)->operations->destroy (page)

/* Representation of current process's memory space.
 * We don't want to force you to obey any specific design for this struct.
 * All designs up to you for this. */
struct supplemental_page_table {
	struct hash *spt_hash; /*추가, 해시테이블*/
	
};

#include "threads/thread.h"
void supplemental_page_table_init (struct supplemental_page_table *spt);
bool supplemental_page_table_copy (struct supplemental_page_table *dst,
		struct supplemental_page_table *src);
void supplemental_page_table_kill (struct supplemental_page_table *spt);
struct page *spt_find_page (struct supplemental_page_table *spt,
		void *va);
bool spt_insert_page (struct supplemental_page_table *spt, struct page *page);
void spt_remove_page (struct supplemental_page_table *spt, struct page *page);

void vm_init (void);
bool vm_try_handle_fault (struct intr_frame *f, void *addr, bool user,
		bool write, bool not_present);

#define vm_alloc_page(type, upage, writable) \
	vm_alloc_page_with_initializer ((type), (upage), (writable), NULL, NULL)
bool vm_alloc_page_with_initializer (enum vm_type type, void *upage,
		bool writable, vm_initializer *init, void *aux);
void vm_dealloc_page (struct page *page);
bool vm_claim_page (void *va);
enum vm_type page_get_type (struct page *page);

#endif  /* VM_VM_H */
