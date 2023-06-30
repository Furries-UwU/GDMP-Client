/*
 * Why have a girlfriend when you can be a girlfriend?
 */

#include "global.hpp"

Global *Global::get() {
    static auto global = new Global;
    return global;
}