// /* anon.c: Implementation of page for non-disk image (a.k.a. anonymous page). */

// #include "vm/vm.h"
// #include "devices/disk.h"

// /* DO NOT MODIFY BELOW LINE */
// static struct disk *swap_disk;
// static bool anon_swap_in (struct page *page, void *kva);
// static bool anon_swap_out (struct page *page);
// static void anon_destroy (struct page *page);

// /* DO NOT MODIFY this struct */
// static const struct page_operations anon_ops = {
// 	.swap_in = anon_swap_in,
// 	.swap_out = anon_swap_out,
// 	.destroy = anon_destroy,
// 	.type = VM_ANON,
// };

// /* Initialize the data for anonymous pages */
// void
// vm_anon_init (void) {
// 	/* TODO: Set up the swap_disk. */
// 	swap_disk = NULL;
// }

// /* Initialize the file mapping */
// bool
// anon_initializer (struct page *page, enum vm_type type, void *kva) {
// 	/* Set up the handler */
// 	page->operations = &anon_ops;

// 	struct anon_page *anon_page = &page->anon;
// }

// /* Swap in the page by read contents from the swap disk. */
// static bool
// anon_swap_in (struct page *page, void *kva) {
// 	struct anon_page *anon_page = &page->anon;
// }

// /* Swap out the page by writing contents to the swap disk. */
// static bool
// anon_swap_out (struct page *page) {
// 	struct anon_page *anon_page = &page->anon;
// }

// /* Destroy the anonymous page. PAGE will be freed by the caller. */
// static void
// anon_destroy (struct page *page) {
// 	struct anon_page *anon_page = &page->anon;
// }
/* anon.c: Implementation of page for non-disk image (a.k.a. anonymous page). */

#include "vm/vm.h"
#include "devices/disk.h"

/* DO NOT MODIFY BELOW LINE */
static struct disk *swap_disk;
static bool anon_swap_in (struct page *page, void *kva);
static bool anon_swap_out (struct page *page);
static void anon_destroy (struct page *page);

/* DO NOT MODIFY this struct */
static const struct page_operations anon_ops = {
	.swap_in = anon_swap_in,
	.swap_out = anon_swap_out,
	.destroy = anon_destroy,
	.type = VM_ANON,
};

/* Initialize the data for anonymous pages */
/* 익명 페이지 하위 시스템 초기화*/
void
vm_anon_init (void) {
	/* TODO: Set up the swap_disk. */
	swap_disk = NULL;
}

/* Initialize the file mapping */
/* page->operation에 있는 역명 페이지에 대한 핸들러 설정
 * 익명 페이지를 초기화하는 함수 */
bool
anon_initializer (struct page *page, enum vm_type type, void *kva) {
	/* Set up the handler */
	page->operations = &anon_ops;
	// page->type = type;
	
	struct anon_page *anon_page = &page->anon;
}

/* Swap in the page by read contents from the swap disk. */
static bool
anon_swap_in (struct page *page, void *kva) {
	struct anon_page *anon_page = &page->anon;
}

/* Swap out the page by writing contents to the swap disk. */
static bool
anon_swap_out (struct page *page) {
	struct anon_page *anon_page = &page->anon;
}

/* Destroy the anonymous page. PAGE will be freed by the caller. */
static void
anon_destroy (struct page *page) {
	struct anon_page *anon_page = &page->anon;
}