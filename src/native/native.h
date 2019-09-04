#ifndef DVLANG_NATIVE_H_
#define DVLANG_NATIVE_H_
#include <unordered_map>
#include <array>
#include <map>
#include <typeinfo>
#include <typeindex>
#include <string>
#include <type_traits>
#include <iostream>
#include <assert.h>
#include <cxxabi.h>
#include <sstream>

#include <stdexcept>

#include "../process/process.h"

namespace NATIVE
{

class MetaFunction
{
public:
  char *name;
  char *signature;
  unsigned int type;
  unsigned int argc;
  void (*func)();
};

class Native
{
  template <typename Func, Func f>
  friend class store_func_helper;

public:
  Native();

  template <typename Func, Func f>
  void store(const std::string &name, MetaFunction meta);

  template <typename Ret, typename... Args>
  Ret call(int func, Args... args);

  template <typename Ret>
  Ret call_wrap(int func);

  template <typename Ret>
  Ret call_wrap2(int func, PROCESS::Cpu *cpu);

  int get(std::string name);

  std::unordered_map<int, std::pair<MetaFunction, std::type_index>> m_func_map;
  std::unordered_map<std::string, int> names;
  //std::unordered_map<int, MetaFunction> metas;
  int index = 0;
};

#define FUNC_MAP_STORE(map, func, meta) (map).store<decltype(&func), &func>(#func, meta);
} // namespace NATIVE

#endif
