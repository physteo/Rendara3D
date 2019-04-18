#pragma once
#include <vector>


namespace memory {


		// SwapArray templated class: allocates memory for maxElements of any class Element.
		// When deleting one of them no deallocation of memory is performed, but the memory slot
		// is reused for new elements.
		template <class Element>
		class SwapArray
		{

		public:
			// Constructor. The user inputs the maximum number elements that are stored.
			SwapArray(size_t maxElements)
			{
				activeElements = 0;
				elements.resize(maxElements);
			}

			// get the number of elements that are being used.
			size_t size() const
			{
				return activeElements;
			}

			// get the element at position index.
			Element& at(size_t index)
			{
				if (index >= activeElements) {
					throw std::out_of_range("System: element requested is not active.");
				}
				return elements.at(index);
			}

			// get const element at position index.
			const Element& at(size_t index) const
			{
				if (index >= activeElements) {
					throw std::out_of_range("System: element requested is not active.");
				}
				return elements.at(index);
			}

			// get the last active element
			Element& back()
			{
				if (activeElements <= 0) {
					throw std::out_of_range("System: number of active elements is zero.");
				}
				return elements.at((size_t)(activeElements - 1));
			}

			// get const last active element
			const Element& back() const
			{
				if (activeElements <= 0) {
					throw std::out_of_range("System: number of active elements is zero.");
				}
				return elements.at((size_t)(activeElements - 1));
			}

			// add an element
			void addBackElement()
			{
				if (activeElements < elements.size()) {
					activeElements++;
				}
				else
				{
					throw std::out_of_range("System: maximum limit of elements in the system exceeded.");
				}
			}


			void push_back(const Element& toadd)
			{
				addBackElement();
				back() = toadd;
			}

			// "delete" element at position index
			void deleteElement(size_t index)
			{
				if (index >= activeElements) {
					throw std::out_of_range("System: element requested is not active.");
				}
				activeElements--;
				swapElements(activeElements, index);
			}

			// pointer to the first element of the vector
			Element* getPointerToFirst()
			{
				return &elements[0];
			}

		private:
			void swapElements(size_t i, size_t j)
			{
				std::swap(elements.at(i), elements.at(j));
			}

		private:
			size_t			  activeElements;
			std::vector<Element> elements;

		};


}

