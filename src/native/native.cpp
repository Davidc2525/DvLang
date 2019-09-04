#include "native.h"
#include <iostream>
#include <tuple>
#include <utility>
#include <vector>

namespace NATIVE
{

//Native *n;

template <typename F, typename T, std::size_t N, std::size_t... Idx>
decltype(auto) apply_impl(F f, T (&t)[N], std::index_sequence<Idx...>)
{
  return f(t[Idx]...);
}

template <typename F, typename T, std::size_t N>
decltype(auto) apply(F f, T (&t)[N])
{
  return apply_impl(f, t, std::make_index_sequence<N>{});
}

//->>
template <typename _Fn, typename _Tuple, std::size_t... _Idx>
constexpr decltype(auto)
__apply_impl(_Fn &&f, _Tuple &&t, std::index_sequence<_Idx...>)
{
  return std::forward<_Fn>(f)(std::get<_Idx>(std::forward<_Tuple>(t))...);
}

template <typename _Fn, typename _Tuple>
constexpr decltype(auto)
apply2(_Fn &&f, _Tuple &&t)
{
  using _Indices =
      std::make_index_sequence<std::tuple_size<std::decay_t<_Tuple>>::value>;
  return __apply_impl(std::forward<_Fn>(f), std::forward<_Tuple>(t),
                      _Indices{});
}
//->>

char *saluda()
{
  return (char *)"hola";
}

int sum(int a, int b)
{
  return a + b;
}

template <typename Func, Func f>
struct store_func_helper;

std::string demangle(const std::string &val)
{
  int status;
  char *realname;

  std::string strname = realname = abi::__cxa_demangle(val.c_str(), 0, 0, &status);
  free(realname);
  return strname;
}

// args will be implicitly converted to arg<T>::type before calling function
// default: convert to const Arg
template <typename Arg, typename snifae = void>
struct arg
{
  using type = const Arg &;
};

// numeric types: convert to double.
template <typename Arg>
struct arg<Arg, typename std::enable_if<std::is_arithmetic<Arg>::value, void>::type>
{
  using type = double;
};

// set more special arg types here.

// Functions stored in the map are first wrapped in a lambda with this signature.
template <typename Ret, typename... Arg>
using func_type = Ret (*)(typename arg<Arg>::type...);

/*template <typename Ret, typename... Args>
Ret call2(int func, Args... args)
{
  using new_func_type = func_type<Ret, Args...>;
  auto &mapVal = n->m_func_map.at(func);

  if (mapVal.second != std::type_index(typeid(new_func_type)))
  {
    std::ostringstream ss;
    ss << "Error calling function " << func << ", function type: "
       << demangle(mapVal.second.name())
       << ", attempted to call with " << demangle(typeid(new_func_type).name());
    throw std::runtime_error(ss.str());
  }

  auto typeCastedFun = (new_func_type)(mapVal.first.func);

  //args will be implicitly converted to match standardized args
  //return apply(typeCastedFun,p);
  return typeCastedFun(std::forward<Args>(args)...);
};*/

Native::Native()
{
  MetaFunction meta;
  meta.argc = 0;
  meta.signature = "()V";
  meta.type = 0;
  meta.name = "saluda";
  FUNC_MAP_STORE(*this, saluda, meta);

  MetaFunction metaSum;
  metaSum.argc = 2;
  metaSum.signature = "(II)I";
  metaSum.type = 1;
  metaSum.name = "sum";
  metaSum.func = (void (*)())sum;
  FUNC_MAP_STORE(*this, sum, metaSum);

  //n = this;
}

template <typename Func, Func f>
void Native::store(const std::string &name, MetaFunction meta)
{
  store_func_helper<Func, f>::call(this, name, meta);
}

template <typename Ret, typename... Args>
Ret Native::call(int func, Args... args)
{
  using new_func_type = func_type<Ret, Args...>;
  auto &mapVal = m_func_map.at(func);

  if (mapVal.second != std::type_index(typeid(new_func_type)))
  {
    std::ostringstream ss;
    ss << "Error calling function " << func << ", function type: "
       << demangle(mapVal.second.name())
       << ", attempted to call with " << demangle(typeid(new_func_type).name());
    throw std::runtime_error(ss.str());
  }

  auto typeCastedFun = (new_func_type)(mapVal.first.func);

  //args will be implicitly converted to match standardized args
  //return apply(typeCastedFun,p);
  return typeCastedFun(std::forward<Args>(args)...);
};

template <typename Ret>
Ret Native::call_wrap(int funcid /* recibe cpu */)
{
  auto &mapVal = m_func_map.at(funcid);
  MetaFunction meta = mapVal.first;
  std::cout << "argc: " << meta.argc << std::endl;
  std::cout << "name: " << meta.name << std::endl;
  std::cout << "signarure: " << meta.signature << std::endl;

  const int c = (meta.argc);
  std::cout << "argc: " << c << std::endl;

  switch (c)
  {
  case 2:
    std::array<int, 3> nc;
    ;
    //int nc[3];
    nc[0] = funcid;
    nc[1] = 1;
    nc[2] = 2;

    return apply2(
        //std::forward<decltype(xs)>
        [this](auto... xs) { return this->call<Ret>(xs...); },
        nc);
    break;
  }

  //return this->call<Ret>(funcid,arg);
}

template <typename Ret>
Ret Native::call_wrap2(int funcid, PROCESS::Cpu *cpu)
{ 
  auto &mapVal = m_func_map.at(funcid);
  MetaFunction meta = mapVal.first;
  std::cout << "argc: " << meta.argc << std::endl;
  std::cout << "name: " << meta.name << std::endl;
  std::cout << "signarure: " << meta.signature << std::endl;

  const int c = (meta.argc);
  std::cout << "argc: " << c << std::endl;

  //code generated with 5 cases.
  switch (c)
  {
    //case 0; generated.
  case 0:
    std::array<int, 1> nc_0;
    nc_0[0] = funcid;
    return apply2(
        //std::forward<decltype(xs)>
        [this](auto... xs) { return this->call<Ret>(xs...); },
        nc_0);
    break;

    //case 1; generated.
  case 1:
    std::array<int, 2> nc_1;

    nc_1[0] = funcid;

    for (int x = 1 - 1; x >= 1; x--)
    {
      nc_1[x] = cpu->estack->Pop();
    }
    //nc_1[1] = 1;
    //nc_1[2] = 2;

    return apply2(
        //std::forward<decltype(xs)>
        [this](auto... xs) { return this->call<Ret>(xs...); },
        nc_1);
    break;

    //case 2; generated.
  case 2:
    std::array<int, 3> nc_2;

    nc_2[0] = funcid;

    for (int x = 2 - 1; x >= 1; x--)
    {
      nc_2[x] = cpu->estack->Pop();
    }
    //nc_2[1] = 1;
    //nc_2[2] = 2;

    return apply2(
        //std::forward<decltype(xs)>
        [this](auto... xs) { return this->call<Ret>(xs...); },
        nc_2);
    break;

    //case 3; generated.
  case 3:
    std::array<int, 4> nc_3;

    nc_3[0] = funcid;

    for (int x = 3 - 1; x >= 1; x--)
    {
      nc_3[x] = cpu->estack->Pop();
    }
    //nc_3[1] = 1;
    //nc_3[2] = 2;

    return apply2(
        //std::forward<decltype(xs)>
        [this](auto... xs) { return this->call<Ret>(xs...); },
        nc_3);
    break;

    //case 4; generated.
  case 4:
    std::array<int, 5> nc_4;

    nc_4[0] = funcid;

    for (int x = 4 - 1; x >= 1; x--)
    {
      nc_4[x] = cpu->estack->Pop();
    }
    //nc_4[1] = 1;
    //nc_4[2] = 2;

    return apply2(
        //std::forward<decltype(xs)>
        [this](auto... xs) { return this->call<Ret>(xs...); },
        nc_4);
    break;
  }
  //return this->call<Ret>(funcid,arg);
}

int Native::get(std::string name)
{
  return names.at(name);
}

template <typename Ret, typename... Args, Ret (*f)(Args...)>
struct store_func_helper<Ret (*)(Args...), f>
{
  static void call(Native *map, const std::string &name, MetaFunction meta)
  {
    using new_func_type = func_type<Ret, Args...>;
    // add a wrapper function, which takes standardized args.
    new_func_type lambda = [](typename arg<Args>::type... args) -> Ret {
      return (*f)(args...);
    };

    (&meta)->func = (void (*)())lambda;

    map->m_func_map.insert(std::make_pair(
        map->index,
        std::make_pair(meta, std::type_index(typeid(lambda)))));

    map->names.insert(std::make_pair(name, map->index));

    //map->metas.insert(std::make_pair(map->index, meta));

    map->index++;
  }
};

} // namespace NATIVE
