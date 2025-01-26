#define UNITY_SUPPORT_64
#define UNITY_OUTPUT_VERBOSE
#include "../tests/harness/unity.h"
#include "../src/lab.h"

static list_t *lst_ = NULL;

static int *alloc_data(int i)
{
  int *rval = (int *)malloc(sizeof(int));
  *rval = i;
  return rval;
}

static void destroy_data(void *data)
{
  free(data);
}

static int compare_to(const void *a, const void *b)
{
  int fst = *(int *)a;
  int snd = *(int *)b;
  return fst - snd;
}

static void populate_list(void)
{
  for (int i = 0; i < 5; i++)
    {
      list_add(lst_, alloc_data(i));
    }
}

void setUp(void) {
  lst_ = list_init(destroy_data, compare_to);
}

void tearDown(void) {
  list_destroy(&lst_);
}

void test_create_destroy(void)
{
  list_t *lst = NULL;
  lst = list_init(destroy_data, compare_to);
  TEST_ASSERT_FALSE(lst == NULL);
  TEST_ASSERT_FALSE(lst->head == NULL);
  TEST_ASSERT_TRUE(lst->size == 0);
  TEST_ASSERT_TRUE(lst->head->data == NULL);
  //Make sure the function pointers are pointing to the correct fuctions
  TEST_ASSERT_TRUE(lst->destroy_data == destroy_data);
  TEST_ASSERT_TRUE(lst->compare_to == compare_to);

  //Make sure we are a circular linked list
  TEST_ASSERT_FALSE(lst->head->next == NULL);
  TEST_ASSERT_FALSE(lst->head->prev == NULL);
  TEST_ASSERT_TRUE(lst->head->next == lst->head->prev);

  list_destroy(&lst);
  TEST_ASSERT_TRUE(lst == NULL);
}


void test_add1(void)
{
  list_add(lst_, alloc_data(1));
  TEST_ASSERT_TRUE(lst_->size == 1);
  //With one node both next and prev should be equal
  TEST_ASSERT_TRUE(lst_->head->next == lst_->head->prev);
  //Make sure we didn't clobber our sentinel node
  TEST_ASSERT_FALSE(lst_->head == lst_->head->next);
  TEST_ASSERT_FALSE(lst_->head == lst_->head->prev);
  TEST_ASSERT_TRUE(lst_->head->data == NULL);

  //Check to make sure our data actually made it into the node
  TEST_ASSERT_TRUE(*((int *)lst_->head->next->data) == 1);
  TEST_ASSERT_TRUE(*((int *)lst_->head->prev->data) == 1);
}

void test_add2(void)
{
  list_add(lst_, alloc_data(1));
  TEST_ASSERT_TRUE(lst_->size == 1);
  list_add(lst_, alloc_data(2));
  TEST_ASSERT_TRUE(lst_->size == 2);

  //With two nodes both next and prev should NOT be equal
  TEST_ASSERT_FALSE(lst_->head->next == lst_->head->prev);
  //Make sure we didn't clobber our sentinel node
  TEST_ASSERT_FALSE(lst_->head == lst_->head->next);
  TEST_ASSERT_FALSE(lst_->head == lst_->head->prev);
  TEST_ASSERT_TRUE(lst_->head->data == NULL);

  //Check to make sure our next and prev have the correct data
  TEST_ASSERT_TRUE(*((int *)lst_->head->next->data) == 2);
  TEST_ASSERT_TRUE(*((int *)lst_->head->prev->data) == 1);
}


void test_removeIndex0(void)
{
  populate_list();
  int *rval = (int *)list_remove_index(lst_, 0);
  TEST_ASSERT_TRUE(lst_->size == 4);
  TEST_ASSERT_TRUE(*rval == 4);
  free(rval);

  node_t *curr = lst_->head->next;
  //List should be 3->2->1->0
  for (int i = 3; i >= 0; i--)
    {
      TEST_ASSERT_TRUE(*((int *)curr->data) == i);
      curr = curr->next;
    }
  curr = lst_->head->prev;
  for (int i = 0; i <= 3; i++)
    {
      TEST_ASSERT_TRUE(*((int *)curr->data) == i);
      curr = curr->prev;
    }
}

void test_removeIndex3(void)
{
  populate_list();
  int *rval = (int *)list_remove_index(lst_, 3);
  TEST_ASSERT_TRUE(lst_->size == 4);
  TEST_ASSERT_TRUE(*rval == 1);
  free(rval);

  node_t *curr = lst_->head->next;
  //List should be 4->3->2->0
  for (int i = 3; i >= 1; i--)
    {
      TEST_ASSERT_TRUE(*((int *)curr->data) == i + 1);
      curr = curr->next;
    }
  //Check the last one
  TEST_ASSERT_TRUE(*((int *)curr->data) == 0);

  //Set the curr back one node so we can check prev links
  curr = curr->prev;
  for (int i = 1; i <= 3; i++)
    {
      TEST_ASSERT_TRUE(*((int *)curr->data) == i + 1);
      curr = curr->prev;
    }
}


void test_removeIndex4(void)
{
  populate_list();
  int *rval = (int *)list_remove_index(lst_, 4);
  TEST_ASSERT_TRUE(lst_->size == 4);
  TEST_ASSERT_TRUE(*rval == 0);
  free(rval);

  node_t *curr = lst_->head->next;
  //List should be 4->3->2->1
  for (int i = 3; i >= 0; i--)
    {
      TEST_ASSERT_TRUE(*((int *)curr->data) == i + 1);
      curr = curr->next;
    }
  curr = lst_->head->prev;
  for (int i = 0; i <= 3; i++)
    {
      TEST_ASSERT_TRUE(*((int *)curr->data) == i + 1);
      curr = curr->prev;
    }
}


void test_invaidIndex(void)
{
  populate_list();
  void *rval = list_remove_index(lst_, 666);
  TEST_ASSERT_TRUE(rval == NULL);
  TEST_ASSERT_TRUE(lst_->size == 5);

  node_t *curr = lst_->head->next;
  //List should be 4->3->2->1->0
  for (int i = 4; i >= 0; i--)
    {
      TEST_ASSERT_TRUE(*((int *)curr->data) ==  i);
      curr = curr->next;
    }

  for (int i = 0; i >= 4; i++)
    {
      TEST_ASSERT_TRUE(*((int *)curr->data) == i);
      curr = curr->prev;
    }
}

void test_removeAll(void)
{
  populate_list();
  //List should be 4->3->2->1->0
  for (int i = 4; i >= 0; i--)
    {
      int *rval = (int *)list_remove_index(lst_, 0);
      TEST_ASSERT_TRUE(*rval == i);
      free(rval);
    }

  //Make sure we back to default
  TEST_ASSERT_FALSE(lst_->head->next == NULL);
  TEST_ASSERT_FALSE(lst_->head->prev == NULL);
  TEST_ASSERT_TRUE(lst_->head->next == lst_->head->prev);
  TEST_ASSERT_TRUE(lst_->size == 0);
}

void test_indexOf0(void)
{
  populate_list();
  //List should be 4->3->2->1->0
  void *data = lst_->head->next->data;
  size_t idx = list_indexof(lst_, data);
  TEST_ASSERT_TRUE(idx == 0);
}

void test_indexOf3(void)
{
  populate_list();
  //List should be 4->3->2->1->0
  void *data = alloc_data(1);
  size_t idx = list_indexof(lst_, data);
  TEST_ASSERT_TRUE(idx == 3);
  free(data);
}

void test_notInList(void)
{
  populate_list();
  void *data = alloc_data(22);
  int idx = list_indexof(lst_, data);
  TEST_ASSERT_EQUAL_INT64(-1, idx);
  free(data);
}

void test_removeFromEmptyList(void)
{
  list_t *list = list_init(destroy_data, compare_to);

  void *data = list_remove_index(list, 0);
  TEST_ASSERT_TRUE(data == NULL); // No data should be returned
  TEST_ASSERT_TRUE(list->size == 0); // Size should remain 0

  list_destroy(&list);
}

void test_addLargeAmountOfData(void)
{
  list_t *list = list_init(destroy_data, compare_to);

  // Add 1000 elements
  for (int i = 0; i < 1000; i++) {
      list_add(list, alloc_data(i));
  }
  TEST_ASSERT_TRUE(list->size == 1000); // Ensure size matches

  // Verify the elements in reverse order (added to front)
  node_t *current = list->head->next;
  for (int i = 999; i >= 0; i--) {
      TEST_ASSERT_TRUE(*(int *)current->data == i);
      current = current->next;
  }

  list_destroy(&list);
}

void test_destroyTwice(void)
{
  list_t *list = list_init(destroy_data, compare_to);

  // Add some elements
  for (int i = 0; i < 3; i++) {
      list_add(list, alloc_data(i));
  }

  // Destroy the list
  list_destroy(&list);
  TEST_ASSERT_TRUE(list == NULL); // Ensure list is NULL after destroy

  // Attempt to destroy again
  list_destroy(&list);
  TEST_ASSERT_TRUE(list == NULL); // Ensure no crashes or changes
}

void test_removeMiddleNode(void)
{
  populate_list(); // Populates list with {4, 3, 2, 1, 0}

  // Remove the middle node (index 2, value 2)
  void *data = list_remove_index(lst_, 2);
  TEST_ASSERT_TRUE(*(int *)data == 2); // Ensure correct node was removed
  free(data);

  // Verify the updated structure
  TEST_ASSERT_TRUE(lst_->size == 4); // Size should decrease
  TEST_ASSERT_TRUE(*(int *)lst_->head->next->next->next->data == 1); // Ensure continuity
  TEST_ASSERT_TRUE(*(int *)lst_->head->next->next->next->prev->data == 3); // Verify prev linkage
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_create_destroy);
  RUN_TEST(test_add1);
  RUN_TEST(test_add2);
  RUN_TEST(test_removeIndex0);
  RUN_TEST(test_removeIndex3);
  RUN_TEST(test_removeIndex4);
  RUN_TEST(test_invaidIndex);
  RUN_TEST(test_removeAll);
  RUN_TEST(test_indexOf0);
  RUN_TEST(test_indexOf3);
  RUN_TEST(test_notInList);
  RUN_TEST(test_removeFromEmptyList);
  RUN_TEST(test_addLargeAmountOfData);
  RUN_TEST(test_destroyTwice);
  RUN_TEST(test_removeMiddleNode);
  return UNITY_END();
}
