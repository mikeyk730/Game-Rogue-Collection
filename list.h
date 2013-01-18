//_detach: Takes an item out of whatever linked list it might be in
_detach(THING **list, THING *item);

//_attach: add an item to the head of a list
_attach(THING **list, THING *item);

//_free_list: Throw the whole blamed thing away
_free_list(THING **ptr);

//new_item: Get a new item with a specified size
THING *new_item();

//talloc: simple allocation of a THING
THING *talloc();

//discard: Free up an item
int discard(THING *item);