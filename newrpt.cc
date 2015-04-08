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
* TODO: finish!
*/
void RPTTable::append(Addr programCounter, Addr memoryAddress) {
	RPTEntry * entry = new RPTEntry(programCounter, MemoryAddress);
        entries[programCounter] = entry;

	if(entries.size() >= MAX_ENTRIES){
		pop(tail);
		entry->next = this.head
		this.head = entry;
	}else if(entries.size()){ // if not empty
		
	}else{// if empty
		this.head = entry
		this.tail = entry
	}
}

RPTEntry * RPTTable::get(Addr programCounter) {
	RPTEntry * entry = entries.find(programCounter);

	table.push_front(entry)
	
	return entry;
}

/*
 * Updates last address on an RPTTable entry
 */
void RPTTable::update(Addr programCounter, Addr memoryAddress){
	entries.find(programCounter)->lastAddress = memoryAddress;
}

/*
 * Adjusts the delta on an RPTTable entry
 */
void RPTTable::adjustDelta(Addr programCounter, Addr memoryAdress){
	RPTEntry * entry = entries.find(programCounter);
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
	issue_pefetch(table->get(stat.pc).lastAddress)
}

/*
 * The simulator calls this function to notify the prefetcher that
 * a prefetch load to address addr has just completed.
 */
void prefetch_complete(Addr addr) {
}


