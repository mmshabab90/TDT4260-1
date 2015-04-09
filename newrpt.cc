#include <map>
#include "interface.hh"

struct RPTEntry {
		RPTEntry(Addr pc);

		Addr programCounter;
		Addr lastAddress;
		int delta;
		RPTEntry * next, * prev;
};

RPTEntry::RPTEntry(Addr pc) : programCounter(pc), lastAddress(0), delta(0), next(NULL), prev(NULL){}

RPTEntry::RPTEntry(Addr pc, Addr la) : programCounter(pc), lastAddress(la), delta(0), next(NULL), prev(NULL){}

class RPTTable {
	public:
		static const int MAX_ENTRIES = 256;

		RPTTable();
		RPTEntry * get(Addr programCounter);
		void append(Addr programCounter);
		void update(Addr programCounter);
		void adjustDelta(Addr programCounter);
	private:
		void push_front(RPTEntry *);
		void pop(RPTEntry *);
		int currentEntries;
		RPTEntry * head, * tail;
		std::map<Addr, RPTEntry *> entries;
};

RPTTable::RPTTable() : currentEntries(0), head(NULL), tail(NULL){}

/*
 * Pushes an RPTTable entry to front of the table(queue)
 */
void RPTTable::push_front(RPTEntry * entry) {
	if(entry->prev == NULL){ // Entry is already head
                return;
        }else if(entry->next == NULL){ // Entry is tail
                this.tail = entry->prev;
                entry->next = this.head;
        }else{
                entry->next->prev = entry->prev;
                entry->prev->next = entry->next;
        }
        this.head = entry;
}

/*
 * Pops the last element (least recently used) from the queue)
 */
void RPTTable::pop(RPTEntry * entry) {
	//TODO: write this

}

/*
 * Inserts a new element at the front of the queue
 */
void RPTTable::append(Addr programCounter, Addr memoryAddress) {
	RPTEntry * entry = new RPTEntry(programCounter, MemoryAddress);
        this.entries[programCounter] = entry;

	if(entries.size() >= MAX_ENTRIES){
		pop(this.tail);
		entry->next = this.head
	}else if(entries.size()){ // if not empty
		entry->next = this.head
	}else{// if empty
		this.tail = entry
	}
	this.head = entry;
}

/*
 * Gets the RPTTable entry corresponding to the instruction in the programCounter
 */
RPTEntry * RPTTable::get(Addr programCounter) {
	RPTEntry * entry = this.entries.find(programCounter);

	this.push_front(entry)
	
	return entry;
}

/*
 * Updates last address on an RPTTable entry
 */
void RPTTable::update(Addr programCounter, Addr memoryAddress){
	this.entries.find(programCounter)->lastAddress = memoryAddress;
}

/*
 * Adjusts the delta on an RPTTable entry
 */
void RPTTable::adjustDelta(Addr programCounter, Addr memoryAdress){
	RPTEntry * entry = this.entries.find(programCounter);
        entry->delta = memoryAddress - entry->lastAddress;
}

static RPTTable * table;

/*
 * The simulator calls this before any memory access to let the prefetcher
 * initialize itself.
 */
void prefetch_init(void) {
	table = new RPTTable;
}

/*
 * The simulator calls this function to notify the prefetcher about
 * a cache access (both hits and misses).
 */
void prefetch_access(AccessStat stat) {
	if(stat.miss) {
		table->adjustDelta(stat.pc)
	}
	table->update(stat.pc)
	issue_pefetch( table->get(stat.pc).lastAddress )
}

/*
 * The simulator calls this function to notify the prefetcher that
 * a prefetch load to address addr has just completed.
 */
void prefetch_complete(Addr addr) {
}


/*
Function Description (things we can use):
void issue prefetch(Addr addr)    Called by the prefetcher to initiate a prefetch
int get prefetch bit(Addr addr)   Is the prefetch bit set for addr?
int set prefetch bit(Addr addr)   Set the prefetch bit for addr
int clear prefetch bit(Addr addr) Clear the prefetch bit for addr
int in cache(Addr addr)           Is addr currently in the L2 cache?
int in mshr queue(Addr addr)      Is there a prefetch request for addr in the MSHR (miss status holding register) queue?
int current queue size(void)      Returns the number of queued prefetch requests
*/
