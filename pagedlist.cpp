#include <bitset>
#include <iostream>
#include <string>

template <typename T, size_t Ps = 32>
class PagedList {
	private:
		consteval static size_t roundUp(size_t n) {
			if ((n & (n - 1)) == 0)
				return n;

			--n;
			n |= n >> 1;
			n |= n >> 2;
			n |= n >> 4;
			n |= n >> 8;
			n |= n >> 16;
			if (4 < sizeof(n))
				n |= n >> 32;
			return n + 1;
		}

		struct Node;
		struct Page;

		struct PageData {
			Node nodes[Ps];
			size_t size;
			size_t last;
			std::bitset<Ps> present;
			Page *prev{};
			Page *next{};
		};

		struct Node {
			alignas(sizeof(T)) char raw[sizeof(T)];
			Node *prev{};
			Node *next{};

			operator T &() {
				return *reinterpret_cast<T *>(raw);
			}

			template <typename... Args>
			T & construct(Args &&...args) {
				return *new (raw) T(std::forward<Args>(args)...);
			}

			void destruct() {
				static_cast<T &>(*this).~T();
			}

			Page * getPage() const {
				const std::uintptr_t address(this);
				return reinterpret_cast<Page *>(address - address % sizeof(PageData));
			}

			size_t getIndex() const {
				const std::uintptr_t address(this);
				return (address % sizeof(PageData)) / sizeof(Node);
			}
		};

		struct alignas(roundUp(sizeof(PageData))) Page: PageData {
			~Page() {
				for (size_t i = 0; i < Ps; ++i) {
					if (this->present.test(i))
						this->nodes[i].destruct();
				}
			}

			template <typename... Args>
			T & construct(size_t index, Args &&...args) {
				if (!this->present.test(index)) {
					++this->size;
					this->present.set(index);
				}

				this->nodes[index].construct(std::forward<Args>(args)...);
			}

			T & at(size_t index) {
				if (!this->present.test(index))
					throw std::out_of_range("Index not contained");

				return static_cast<T &>(this->nodes[index]);
			}

			void erase(size_t index) {
				if (!this->present.test(index))
					throw std::out_of_range("Index not contained");

				searchLast(index);
				this->nodes[index].destruct();
				--this->size;
				this->present.reset(index);
			}

			void searchLast(size_t index) {
				if (this->last != index || this->last == 0)
					return;

				for (--this->last; this->last != 0 && this->present.test(index); --this->last);

				if (this->last == 0 && this->present.test(0)) {
					if (index == Ps - 1) {
						this->last = index;
					} else {
						for (this->last = index + 1; this->last != Ps - 1 && this->present.test(index); ++this->last);
					}
				}
			}
		};

		std::vector<Page *> pages;

	public:
		class iterator {
			private:
				Node *node{};

			public:
				iterator() = default;

				iterator(Node *node_):
					node(node_) {}

				inline T & operator*() const {
					return static_cast<T &>(*node);
				}

				inline T * operator->() const {
					return &static_cast<T &>(*node);
				}

				inline bool operator==(const iterator &other) const {
					return node == other.node;
				}

				inline bool operator!=(const iterator &other) const {
					return node != other.node;
				}

				iterator & operator++() {
					node = node->next;
					return *this;
				}

				iterator & operator--() {
					node = node->prev;
					return *this;
				}

				iterator operator++(int) {
					iterator original{*this};
					node = node->next;
					return original;
				}

				iterator operator--(int) {
					iterator original{*this};
					node = node->prev;
					return original;
				}
		};

		PagedList() = default;

		~PagedList() {
			for (Page *page: pages)
				delete page;
		}

		void insert(iterator iter, const T &item) {
			Node *node = iter->node;
			size_t index = node->getIndex();

		}
};

int main() {
	PagedList<int> list;
}
