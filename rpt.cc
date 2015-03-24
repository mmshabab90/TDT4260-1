#include <map>
#include "interface.hh"

struct RPTEntry {
		RPTEntry(Addr pc);
		void miss(Addr addr);

		Addr pc, lastAddress;
		int delta;
		RPTEntry * next, * prev;
};

RPTEntry::RPTEntry(Addr pc) : pc(pc), lastAddress(0), delta(0), next(0), prev(0){}

void RPTEntry::miss(Addr addr) {
	int newDelta = addr - lastAddress;

	if(delta == newDelta && !in_cache(addr + delta)) {
		issue_prefetch(addr + delta);
	}

	delta = newDelta;
	lastAddress = addr;
}

class RPTTable {
	public:
		static const int MAX_ENTRIES = 256;

		RPTTable();
		RPTEntry * get(Addr pc);
	private:
		int currentEntries;
		RPTEntry * head, * tail;
		std::map<Addr, RPTEntry *> entryMap;
};

RPTTable::RPTTable() : currentEntries(0), head(0), tail(0){}

RPTEntry * RPTTable::get(Addr pc) {
	if(entryMap.find(pc) == entryMap.end()) {
		RPTEntry * newEntry = new RPTEntry(pc);
		if(currentEntries == MAX_ENTRIES) {
			RPTEntry * oldest = tail;
			tail = oldest->next;
			tail->prev = 0;
			entryMap.erase(oldest->pc);
			delete oldest;
		}

		if(head != 0) {
			head->next = newEntry;
			newEntry->prev = head;
		} else {
			tail = newEntry;
			newEntry->next = 0;
			newEntry->prev = 0;
		}

		head = newEntry;
		entryMap[pc] = newEntry;

		if(currentEntries < MAX_ENTRIES) {
			++currentEntries;
		}
	} else {
		RPTEntry * entry = entryMap[pc];

		if(entry != head) {
			entry->next->prev = entry->prev;
			if(entry->prev) {
				entry->prev->next = entry->next;
			} else {
				tail = entry->next;
			}

			entry->next = 0;
			head->next = entry;
			entry->prev = head;
			head = entry;
		}
	}

	return entryMap[pc];
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
		stat.mem_addr &= -BLOCK_SIZE;

		RPTEntry * entry = table->get(stat.pc);
		entry->miss(stat.mem_addr);
	}
}

/*
 * The simulator calls this function to notify the prefetcher that
 * a prefetch load to address addr has just completed.
 */
void prefetch_complete(Addr addr) {
}


