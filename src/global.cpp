/* ========================================================
 * Why have a girl when you can have an imaginary fox girl?
 * ========================================================
 */

#include "global.hpp"

Global *Global::get() {
    static auto global = new Global;
    return global;
}