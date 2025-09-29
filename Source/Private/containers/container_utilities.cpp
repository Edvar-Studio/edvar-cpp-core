#include "containers/container_utilities.h"
#include "diagnostics/assertion.h"

namespace edvar::container::utilities {
void check_bounds(int min_inclusive, int max_inclusive, int index) {
    edvar::diagnostics::check_printf(index >= min_inclusive && index <= max_inclusive,
                                     L"bounds check failure: [%d, %d] but input was %d!", min_inclusive, max_inclusive,
                                     index);
}
} // namespace edvar::container::utilities