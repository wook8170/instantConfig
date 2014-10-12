
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>
#include <tuple>
#include <string>
#include <unordered_map>
#include <functional>

/*****************************************************
    optional
******************************************************/
namespace std{
template <class T>
struct optional{};
}

/*****************************************************
    forward decl
******************************************************/

namespace icfg{

  class ConfigError;
  class LengthError;
    
  namespace internal{
  
    template <uint64_t,uint64_t,uint64_t,uint64_t>
    struct settingName;

    template <typename... Params>
    struct configDef;
    template <typename... Params>
    struct settingDef;
    template <typename... Params>
    struct sectionDef;
    template <typename... Params>
    struct mapDef;
    template <typename SettingName, typename... Params>
    struct switchDef;
    template <typename... Params>
    struct caseDef;

    template <typename... Params>
    sectionDef<Params...> make_sectionDef(const std::tuple<Params...>& params){
      return sectionDef<Params...>(params);
    }

    template <typename Name, typename type>
    struct settingResult;
    template <typename... Values>
    struct groupResult;
    template <typename... Params>
    struct configResult;
    template <typename Name, typename... Cases>
    struct switchResult;
    template <typename Name, typename... Settings>
    struct caseResult;

    template <typename Param>
    struct TuplifyOne;
    template <typename Param>
    auto tuplifyOne(const Param& param) -> decltype(TuplifyOne<Param>::tuplify(param));

    struct empty_t{
      typedef empty_t myType;
    };

    template <typename... Params>
    struct MakeConfigResult;

    template <typename... Params>
    struct MakeSettingResult;

    template <typename... Params>
    struct MakeSectionResult;

    template <typename... Params>
    struct MakeMapResult;

    template <typename... Params>
    struct MakeSwitchResult;

    template <typename... Params>
    struct MakeCaseResult;

    template <typename LoaderT>
    struct ConfigValueLoader_t;

    template <typename needle, typename... values>
    struct ExtractSettingFromName;

    template <typename... Params>
    struct ExtractSettingName;

    template <typename Name, typename... Params>
    struct ExtractCaseDefFromName;

    template <typename Name, typename... Params>
    struct ExtractCaseFromName;

    template <typename... Params>
    struct ExtractStaticSettingDefParameters;

    template <size_t Index, size_t Size>
    struct ValidateParameters;

    template <typename ConstraintType, typename ResultType>
    struct ApplyConstraintCheck;
    
  }
  
  template <typename... Params>
  internal::configDef<Params...> config(Params... params);

  template <typename... Params>
  internal::settingDef<Params...> setting(Params... params);

  template <typename... Params>
  auto section(Params... params) -> decltype(internal::make_sectionDef(std::tuple_cat(internal::tuplifyOne(params)...)));

  template <typename... Params>
  internal::mapDef<Params...> map(Params... params);

  template <uint64_t Q1, uint64_t Q2, uint64_t Q3, uint64_t Q4, typename... Params>
  internal::switchDef<internal::settingName<Q1,Q2,Q3,Q4>,Params...> switchOn(internal::settingName<Q1,Q2,Q3,Q4>, Params... params);

  template <typename... Params>
  internal::caseDef<Params...> caseOf(Params... params);

  template <typename... Params>
  std::tuple<Params...> include(const internal::configDef<Params...>& param);

  template <typename FTorT>
  auto include(const FTorT&& functor) -> decltype(functor().content);
  
/*****************************************************
    compile-time string storage : up to 32 chars
******************************************************/

  namespace internal{
    
    template <uint64_t Q1,uint64_t Q2,uint64_t Q3,uint64_t Q4>
    struct settingName{
      constexpr settingName(const char*){}
      
      settingName(){}
      
      static std::string getString(){
        std::string result;
        for(int d=0 ; d<8 ; ++d){
          char c = static_cast<char>((Q1>>(56-8*d))&0xff);
          if (c=='\0'){
            return result;
          }
          result.push_back(c);
        }
        for(int d=0 ; d<8 ; ++d){
          char c = static_cast<char>((Q2>>(56-8*d))&0xff);
          if (c=='\0'){
            return result;
          }
          result.push_back(c);
        }
        for(int d=0 ; d<8 ; ++d){
          char c = static_cast<char>((Q3>>(56-8*d))&0xff);
          if (c=='\0'){
            return result;
          }
          result.push_back(c);
        }
        for(int d=0 ; d<8 ; ++d){
          char c = static_cast<char>((Q4>>(56-8*d))&0xff);
          if (c=='\0'){
            return result;
          }
          result.push_back(c);
        }
        return result;
      }
    };

    constexpr uint64_t encodeStr(char const* str, size_t sz, int8_t index=0, int8_t off=0){
      return (sz<33 && index<sz && off<8) ? ((static_cast<uint64_t>(str[index])<<(56-8*off))|encodeStr(str, sz, index+1, off+1)): 0;
    }

    template <typename T>
    struct isName{
      static const bool value = false;
    };

    template <uint64_t Q1,uint64_t Q2,uint64_t Q3,uint64_t Q4>
    struct isName<settingName<Q1,Q2,Q3,Q4>>{
      static const bool value = true;
    };
    
  }

#define STG_NAME(name) icfg::internal::settingName<icfg::internal::encodeStr(name, sizeof(name)), icfg::internal::encodeStr(name, sizeof(name), 8),icfg::internal::encodeStr(name, sizeof(name), 16),icfg::internal::encodeStr(name, sizeof(name), 24)>(name)
/*****************************************************
    Error classes
******************************************************/

  class ConfigError{

  public:
    ConfigError(const std::string& pWhat)
    : mWhat(pWhat){}
    
    virtual ~ConfigError(){}
    
    virtual const std::string what() const {
      return mWhat;
    }
    
    void setFailingSettingName(const std::string& name){
      mFailingSetting = name;
    }
    
    const std::string& getFailingSettingName() const {
      return mFailingSetting;
    }
    
  private:
    std::string mWhat;
    std::string mFailingSetting;
  };

/*****************************************************
    Setting parameters
******************************************************/

  namespace internal{
    struct stringTag{};// -> std::string

    template<class T>
    struct listTag{};//-> std::vector
    
    struct optionalTag{};
    
    struct requiredTag{};


  }
  extern internal::stringTag string;

  template <typename T>
  internal::listTag<T> list(T){}
  
  extern internal::optionalTag optional;
  extern internal::requiredTag required;
    
/*****************************************************
    Constraints
******************************************************/

  namespace internal{
    namespace LengthConstraint_internal{
      namespace LengthConstraintType{
        enum LengthConstraintType{
          Equal,
          LessOrEqual,
          Less,
          GreaterOrEqual,
          Greater,
          Different
        };
      }
      
      struct Equal{
        static bool test(size_t ref, size_t actual){
          return ref==actual;
        }
        static constexpr const char* Description = "equal";
        static const internal::LengthConstraint_internal::LengthConstraintType::LengthConstraintType Type = internal::LengthConstraint_internal::LengthConstraintType::Equal;
      };
      struct GreaterEqual{
        static bool test(size_t ref, size_t actual){
          return actual>=ref;
        }
        static constexpr const char* Description = "greater or equal";
        static const internal::LengthConstraint_internal::LengthConstraintType::LengthConstraintType Type = internal::LengthConstraint_internal::LengthConstraintType::GreaterOrEqual;
      };
      struct Greater{
        static bool test(size_t ref, size_t actual){
          return actual>ref;
        }
        static constexpr const char* Description = "greater";
        static const internal::LengthConstraint_internal::LengthConstraintType::LengthConstraintType Type = internal::LengthConstraint_internal::LengthConstraintType::Greater;
      };
      struct LessEqual{
        static bool test(size_t ref, size_t actual){
          return actual<=ref;
        }
        static constexpr const char* Description = "less or equal";
        static const internal::LengthConstraint_internal::LengthConstraintType::LengthConstraintType Type = internal::LengthConstraint_internal::LengthConstraintType::LessOrEqual;
      };
      struct Less{
        static bool test(size_t ref, size_t actual){
          return actual<ref;
        }
        static constexpr const char* Description = "less";
        static const internal::LengthConstraint_internal::LengthConstraintType::LengthConstraintType Type = internal::LengthConstraint_internal::LengthConstraintType::Less;
      };
      struct Different{
        static bool test(size_t ref, size_t actual){
          return actual!=ref;
        }
        static constexpr const char* Description = "different";
        static const internal::LengthConstraint_internal::LengthConstraintType::LengthConstraintType Type = internal::LengthConstraint_internal::LengthConstraintType::Different;
      };
    }


    template <typename ConstraintT>
    struct LengthConstraint{
      
      size_t mRef;
      
      bool validate(size_t pActual) const{
        return ConstraintT::test(mRef, pActual);
      }
      
      ConfigError getError(size_t pActual) const{
        std::string constraintDescription = ConstraintT::Type==internal::LengthConstraint_internal::LengthConstraintType::Equal? "equal" :
                                            ConstraintT::Type==internal::LengthConstraint_internal::LengthConstraintType::Different? "different from" :
                                            ConstraintT::Type==internal::LengthConstraint_internal::LengthConstraintType::Greater? "greater than" :
                                            ConstraintT::Type==internal::LengthConstraint_internal::LengthConstraintType::GreaterOrEqual? "greater or equal to" :
                                            ConstraintT::Type==internal::LengthConstraint_internal::LengthConstraintType::Less? "less than" :
                                            "less or equal to";
        return ConfigError("Length constraint failed : size must be "+constraintDescription+" "+std::to_string(mRef)+", actual is "+std::to_string(pActual));
      }
      
    };
    struct lengthTag{
      LengthConstraint<internal::LengthConstraint_internal::GreaterEqual> operator>=(std::size_t rhs){
        return LengthConstraint<internal::LengthConstraint_internal::GreaterEqual>{rhs};
      }
      
      LengthConstraint<internal::LengthConstraint_internal::Greater> operator>(std::size_t rhs){
        return LengthConstraint<internal::LengthConstraint_internal::Greater>{rhs};
      }
      
      LengthConstraint<internal::LengthConstraint_internal::LessEqual> operator<=(std::size_t rhs){
        return LengthConstraint<internal::LengthConstraint_internal::LessEqual>{rhs};
      }
      
      LengthConstraint<internal::LengthConstraint_internal::Less> operator<(std::size_t rhs){
        return LengthConstraint<internal::LengthConstraint_internal::Less>{rhs};
      }
      
      LengthConstraint<internal::LengthConstraint_internal::Equal> operator==(std::size_t rhs){
        return LengthConstraint<internal::LengthConstraint_internal::Equal>{rhs};
      }
      
      LengthConstraint<internal::LengthConstraint_internal::Different> operator!=(std::size_t rhs){
        return LengthConstraint<internal::LengthConstraint_internal::Different>{rhs};
      }
    };
    
  }
  
  static internal::lengthTag length;
  
/*****************************************************
    index_sequence_for : until c++14, define a helper 
    that creates a integer_sequence<0,..,N-1>
    from the number of parameters to index_sequence_for
******************************************************/

  namespace internal{
    template<size_t ...>
    struct integer_sequence { };

    template<int N, size_t ...S>
    struct gens : gens<N-1, N-1, S...> { };

    template<size_t ...S>
    struct gens<0, S...> {
      typedef integer_sequence<S...> type;
    };

    template <typename... Params>
    using index_sequence_for = typename gens<sizeof...(Params)>::type;
    
/*****************************************************
    MakeSettingDefDynParametersTuple : create the
    tuple that will be member of settingDef
    for containing dynamically-parameterized
    elements (like constraints)
******************************************************/

    template <typename T>
    struct IsSettingDefDynParameter;

    template <typename... Params>
    struct ConcatSettingDefDynParameters;

    template <typename T, typename... Params>
    struct ConcatSettingDefDynParameters<T, Params...>{
      static auto concat(T& first, Params... params) -> decltype(std::tuple_cat(IsSettingDefDynParameter<T>::extract(first), ConcatSettingDefDynParameters<Params...>::concat(params...))){
        return std::tuple_cat(IsSettingDefDynParameter<T>::extract(first), ConcatSettingDefDynParameters<Params...>::concat(params...));
      }
    };

    template <typename T>
    struct ConcatSettingDefDynParameters<T>{
      static auto concat(T& first) -> decltype(std::tuple_cat(IsSettingDefDynParameter<T>::extract(first))){
        return std::tuple_cat(IsSettingDefDynParameter<T>::extract(first));
      }
    };

    template <>
    struct ConcatSettingDefDynParameters<>{
      static std::tuple<> concat() {
        return std::tuple<>();
      }
    };

    //return empty tuple if not to be retained for dynamic parameters
    template <typename T>
    struct IsSettingDefDynParameter{
      static auto extract(const T& param) -> std::tuple<>{
        return std::tuple<>();
      }
    };

    template <typename T>
    struct IsSettingDefDynParameter<LengthConstraint<T>>{
      static auto extract(const LengthConstraint<T>& param) -> std::tuple<LengthConstraint<T>>{
        return std::tuple<LengthConstraint<T>>(param);
      }
    };

    template <typename... Params>
    struct IsSettingDefDynParameter<std::tuple<Params...>>{
      static auto extract(const std::tuple<Params...>& param) -> decltype(IsSettingDefDynParameterOnTuple(param, index_sequence_for<Params...>{})){
        return IsSettingDefDynParameterOnTuple(param, index_sequence_for<Params...>{});
      }
    };

    template <typename Tuple, size_t... Idx>
    auto IsSettingDefDynParameterOnTuple(const Tuple& tup, integer_sequence<Idx...>) 
    -> decltype(std::tuple_cat(IsSettingDefDynParameter<typename std::tuple_element<Idx, Tuple>::type>::extract(std::get<Idx>(tup))...)){
      return std::tuple_cat(IsSettingDefDynParameter<typename std::tuple_element<Idx, Tuple>::type>::extract(std::get<Idx>(tup))...);
    }

    template <typename... Params>
    auto MakeSettingDefDynParametersTuple(Params... params) -> decltype(std::tuple_cat(IsSettingDefDynParameter<Params>::extract(params)...)){
      return std::tuple_cat(IsSettingDefDynParameter<Params>::extract(params)...);
    }

/*****************************************************
    config 
******************************************************/

    template <size_t index, size_t size>
    struct LoadSetting;

    template <typename... Params>
    struct configDef{
      template <typename ConfigBackend>
      friend struct ConfigValueLoader_t;

      typedef typename MakeConfigResult<Params...>::type result_type;
      
      template <typename... Ps>
      friend std::tuple<Ps...> icfg::include(const icfg::internal::configDef<Ps...>& param);
      
    private:
      std::tuple<Params...> content;

    public:
      configDef(std::tuple<Params...> params)
      : content(params){
      
      }
      
      template <class ConfigBackend>
      result_type load(const ConfigBackend& loader) const{
        result_type result;
        std::vector<ConfigError> errors = ConfigValueLoader_t<ConfigBackend>::load(loader, result, *this);
        result.setErrors(errors);
        return result;
      }
      
    private:
      std::vector<ConfigError> validate(const result_type& ) const {
        return std::vector<ConfigError>();
      }
    };

/*****************************************************
    setting 
******************************************************/

    template <typename... Params>
    struct settingDef{
      template <typename ConfigBackend>
      friend struct ConfigValueLoader_t;
      
      typedef typename MakeSettingResult<Params...>::type result_type;
      
      template <typename... Ps>
      settingDef(std::tuple<Ps...> params)
      : parameters(internal::MakeSettingDefDynParametersTuple(params)){
      }
      
    private:
      
      std::vector<ConfigError> validate(const result_type& result) const {
        std::vector<ConfigError> errors;
        ValidateParameters<0,std::tuple_size<decltype(parameters)>::value>::validate(parameters, result.value, errors);
        for(auto& err : errors)   {
          err.setFailingSettingName(result_type::myName::getString());
        }
        return errors;
      }
      
    private:
      decltype(MakeSettingDefDynParametersTuple(std::declval<Params>()...)) parameters;
      
    };

/*****************************************************
    section 
******************************************************/

    template <typename... Params>
    struct sectionDef{
      template <typename ConfigBackend>
      friend struct ConfigValueLoader_t;
      
      typedef typename MakeSectionResult<Params...>::type result_type;
      
    private:
      std::tuple<Params...> content;

    public:
      sectionDef(const std::tuple<Params...>& params)
      : content(params){
      
      }
      
    private:
      std::vector<ConfigError> validate(const result_type& ) const {
        return std::vector<ConfigError>();
      }
    };

/*****************************************************
    map
******************************************************/

    template <typename... Params>
    struct mapDef{
      template <typename ConfigBackend>
      friend struct ConfigValueLoader_t;
        
      typedef typename MakeMapResult<Params...>::type result_type;
      
    private:
      std::tuple<Params...> content;

    public:
      mapDef(const std::tuple<Params...>& params)
      : content(params){
      
      }
      
    private:
      std::vector<ConfigError> validate(const result_type& ) const {
        return std::vector<ConfigError>();
      }

    };

/*****************************************************
    switch and case
******************************************************/

    //matching name iteration
    template <typename CaseName, typename... CaseParams, typename... OtherCases>
    struct ExtractCaseDefFromName<CaseName, caseDef<CaseName,CaseParams...>, OtherCases...>{
      typedef caseDef<CaseName,CaseParams...> result;
      static const int index=0;
    };

    //matching name terminal
    template <typename CaseName, typename... CaseParams>
    struct ExtractCaseDefFromName<CaseName, caseDef<CaseName,CaseParams...>>{
      typedef caseDef<CaseName,CaseParams...> result;
      static const int index=0;
    };

    //not-matching name iteration
    template <typename Name, typename Name1, typename... CaseParams, typename... Params>
    struct ExtractCaseDefFromName<Name, caseDef<Name1,CaseParams...>, Params...>{
      typedef typename ExtractCaseDefFromName<Name, Params...>::result result;
      static const int index=ExtractCaseDefFromName<Name, Params...>::index+1;
    };

    //not-matching name terminal
    template <typename Name, typename Name1, typename... CaseParams>
    struct ExtractCaseDefFromName<Name, caseDef<Name1,CaseParams...>>{
      typedef empty_t result;
      static const int index=0;
    };

    template <typename SettingName, typename... Params>
    struct switchDef{
      template <typename ConfigBackend>
      friend struct ConfigValueLoader_t;
        
      typedef typename MakeSwitchResult<SettingName, Params...>::type result_type;
      
    private:
      std::tuple<Params...> content;

    public:
      switchDef(const std::tuple<Params...>& params)
      : content(params){
      
      }
      
      template<typename settingName>
      auto getCase(settingName)const -> typename std::add_lvalue_reference<typename std::add_const<typename ExtractCaseDefFromName<settingName,Params...>::result>::type>::type {
        return std::get<ExtractCaseDefFromName<settingName,Params...>::index>(content);
      }
      
    private:
      std::vector<ConfigError> validate(const result_type& ) const {
        return std::vector<ConfigError>();
      }

    };

    template <typename... Params>
    struct caseDef{
      template <typename ConfigBackend>
      friend struct ConfigValueLoader_t;
        
      typedef typename MakeCaseResult<Params...>::type result_type;
      
    private:
      std::tuple<Params...> content;

    public:
      caseDef(const std::tuple<Params...>& params)
      : content(params){
      
      }
      
    private:
      std::vector<ConfigError> validate(const result_type& ) const {
        return std::vector<ConfigError>();
      }

    };
    
  }
/*****************************************************
    Setting creation functions
******************************************************/
  
  template <typename... Params>
  internal::configDef<Params...> config(Params... params){
    return internal::configDef<Params...>(std::tuple_cat(internal::tuplifyOne(params)...));
  }

  template <typename... Params>
  internal::settingDef<Params...> setting(Params... params){
    return internal::settingDef<Params...>(std::tuple_cat(internal::tuplifyOne(params)...));
    //return settingDef<Params...>(std::tuple_cat(tuplifyOne(params)...));
  }

  template <typename... Params>
  auto section(Params... params) -> decltype(internal::make_sectionDef(std::tuple_cat(internal::tuplifyOne(params)...))){
    return internal::make_sectionDef(std::tuple_cat(internal::tuplifyOne(params)...));
  }

  template <typename... Params>
  internal::mapDef<Params...> map(Params... params){
    return internal::mapDef<Params...>(std::tuple_cat(internal::tuplifyOne(params)...));
  }

  template <uint64_t Q1, uint64_t Q2, uint64_t Q3, uint64_t Q4, typename... Params>
  internal::switchDef<internal::settingName<Q1,Q2,Q3,Q4>,Params...> switchOn(internal::settingName<Q1,Q2,Q3,Q4>, Params... params){
    return internal::switchDef<internal::settingName<Q1,Q2,Q3,Q4>,Params...>(std::tuple_cat(internal::tuplifyOne(params)...));
  }

  template <typename... Params>
  internal::caseDef<Params...> caseOf(Params... params){
    return internal::caseDef<Params...>(std::tuple_cat(internal::tuplifyOne(params)...));
  }

  //include() configDef<...>
  template <typename... Params>
  std::tuple<Params...> include(const internal::configDef<Params...>& param){
    return param.content;
  }

  template <typename FTorT>
  auto include(const FTorT&& functor) -> decltype(functor().content){
    return functor().content;
  }

/*****************************************************
    Tuplify : wraps all arguments into tuple if not
    already a tuple
******************************************************/

  namespace internal{
      
    template <typename Param>
    struct TuplifyOne{
      static std::tuple<Param> tuplify(const Param& param){
        return std::tuple<Param>(param);
      }
    };

    template <typename... Param>
    struct TuplifyOne<std::tuple<Param...>>{
      static const std::tuple<Param...>& tuplify(const std::tuple<Param...>& param){
        return param;
      }
    };

    template <typename Param>
    auto tuplifyOne(const Param& param) -> decltype(TuplifyOne<Param>::tuplify(param)){
      return TuplifyOne<Param>::tuplify(param);
    }

    // template <typename Param, typename... Params>
    // struct Tuplify<Param,Params...>{
      // static auto tuplify(Param&& param, Params&&... params) -> decltype(std::tuple_cat(tuplifyOne(std::forward(param)), Tuplify<Params>::tuplify(std::forward(params...)))){
        // return std::tuple_cat(tuplifyOne(std::forward(param))), Tuplify<Params>::tuplify(std::forward(params...));
      // }
    // }

    // template <typename Param>
    // struct Tuplify<Param>{
      // static auto tuplify(Param&& param, Params&&... params) -> decltype(std::tuple_cat(tuplifyOne(std::forward(param)))){
        // return tuplifyOne(std::forward(param));
      // }
    // }

    template <typename... Params>
    auto tuplify(const Params&... params) -> decltype(std::tuple_cat(tuplifyOne(params)...)){
      return std::tuple_cat(tuplifyOne(params)...);
    }
    
/*****************************************************
    ExtractSettingName : retrieve the setting name
    off the parameter pack
******************************************************/

    template <typename Param1, typename... Params>
    struct ExtractSettingName<Param1,Params...>{
      typedef typename ExtractSettingName<Params...>::result result;
    };

    template <uint64_t Q1, uint64_t Q2, uint64_t Q3, uint64_t Q4, typename... Params>
    struct ExtractSettingName<settingName<Q1,Q2,Q3,Q4>, Params...>{
      typedef settingName<Q1,Q2,Q3,Q4> result;
    };

    template <typename Param1>
    struct ExtractSettingName<Param1>{
      typedef settingName<0,0,0,0> result;
    };

/*****************************************************
    ExtractSettingType : computes the type based
    on type tags and occurence tags
******************************************************/

    template <typename SettingT, typename FirstParam, typename... Params>
    struct ExtractOccurence{
      typedef typename ExtractOccurence<SettingT,Params...>::type type;
    };

    template <typename SettingT, typename FirstParam>
    struct ExtractOccurence<SettingT, FirstParam>{
      typedef SettingT type;
    };

    template <typename SettingT, typename... Params>
    struct ExtractOccurence<SettingT, optionalTag, Params...>{
      typedef std::optional<SettingT> type;
    };

    template <typename SettingT>
    struct ExtractOccurence<SettingT, optionalTag>{
      typedef std::optional<SettingT> type;
    };

    template <typename SettingT, typename... Params>
    struct ExtractOccurence<SettingT, requiredTag, Params...>{
      typedef SettingT type;
    };

    template <typename SettingT>
    struct ExtractOccurence<SettingT, requiredTag>{
      typedef SettingT type;
    };

    template <typename FirstParam, typename... Params>
    struct ExtractResultType{
      typedef typename ExtractResultType<Params...>::type type;
    };

    #define DECLARE_TYPE_EXTRACTOR_FOR(TypeDesc, DestType)    \
    template <typename... Params>                             \
    struct ExtractResultType<TypeDesc, Params...>{            \
      typedef DestType type;                                  \
    };                                                        \
    template <>                                               \
    struct ExtractResultType<TypeDesc>{                       \
      typedef DestType type;                                  \
    }                                                       

    DECLARE_TYPE_EXTRACTOR_FOR(stringTag, std::string);
    DECLARE_TYPE_EXTRACTOR_FOR(uint8_t, uint8_t);
    DECLARE_TYPE_EXTRACTOR_FOR(int8_t, int8_t);
    DECLARE_TYPE_EXTRACTOR_FOR(uint16_t, uint16_t);
    DECLARE_TYPE_EXTRACTOR_FOR(int16_t, int16_t);
    DECLARE_TYPE_EXTRACTOR_FOR(uint32_t, uint32_t);
    DECLARE_TYPE_EXTRACTOR_FOR(int32_t, int32_t);
    DECLARE_TYPE_EXTRACTOR_FOR(uint64_t, uint64_t);
    DECLARE_TYPE_EXTRACTOR_FOR(int64_t, int64_t);
    template <typename SubT, typename... Params>
    struct ExtractResultType<listTag<SubT>, Params...>{
      typedef std::vector<typename ExtractResultType<SubT>::type> type;
    };
    template <typename SubT>
    struct ExtractResultType<listTag<SubT>>{
      typedef std::vector<typename ExtractResultType<SubT>::type> type;
    };

    template <typename... Params>
    struct ExtractSettingType{
      typedef typename ExtractOccurence<typename ExtractResultType<Params...>::type, Params...>::type type;
      static_assert(!std::is_same<type, empty_t>::value, "Could not deduce setting result type");
    };

/*****************************************************
    MakeSettingResult : compute the result type
    of the given setting
******************************************************/

    template <typename... Params>
    struct MakeSettingResult{
      typedef settingResult<typename ExtractSettingName<Params...>::result,typename ExtractSettingType<Params...>::type> type;
    };

/*****************************************************
    MakeSectionResult : compute the result type
    of the given section
******************************************************/

    template <typename... Params>
    struct MakeConfigResultWithoutName;

    template <typename... Params>
    struct MakeSectionResult{
      typedef settingResult<typename ExtractSettingName<Params...>::result,typename MakeConfigResultWithoutName<Params...>::type> type;
    };
   
/*****************************************************
    MakeMapResult : compute the result type
    of the given map
******************************************************/

    template <typename... Params>
    struct MakeMapResult{
      typedef settingResult<typename ExtractSettingName<Params...>::result,std::unordered_map<std::string, typename MakeConfigResultWithoutName<Params...>::type>> type;
    };

/*****************************************************
    UnpackConfigParameter : dispatch to different
    handlers depending on the name of the parameter
******************************************************/
      
    template <template <typename...> class ParamT, typename... Params>
    struct UnpackConfigParameter{
      typedef std::tuple<empty_t> type;
    };

    //match a "settingDef" tag
    template <typename... Params>
    struct UnpackConfigParameter<settingDef, Params...>{
      typedef std::tuple<typename MakeSettingResult<Params...>::type> type;
    };

    //match a "sectionDef" tag
    template <typename... Params>
    struct UnpackConfigParameter<sectionDef, Params...>{
      typedef std::tuple<typename MakeSectionResult<Params...>::type> type;
    };

    //match a "mapDef" tag
    template <typename... Params>
    struct UnpackConfigParameter<mapDef, Params...>{
      typedef std::tuple<typename MakeMapResult<Params...>::type> type;
    };

    //match a switchDef tag
    template <typename... Params>
    struct UnpackConfigParameter<switchDef, Params...>{
      typedef std::tuple<typename MakeSwitchResult<Params...>::type> type;
    };

    //match a caseDef tag
    template <typename... Params>
    struct UnpackConfigParameter<caseDef, Params...>{
      typedef std::tuple<typename MakeCaseResult<Params...>::type> type;
    };

    //match a configDef tag
    template <typename... Params>
    struct UnpackConfigParameter<configDef, Params...>{
      typedef decltype(MakeParametersResults(std::declval<Params>()...)) type;
    };

    //generate the result of one definition node
    template <template <typename...> class ParamT, typename... Params>
    auto UnpackConfigParameterATD(ParamT<Params...>) -> typename UnpackConfigParameter<ParamT,Params...>::type {}

    //generate the results of Params... merged into a tuple
    template <typename... Params>
    auto MakeParametersResults(Params... params) -> decltype(std::tuple_cat(std::declval<decltype(UnpackConfigParameterATD(params))>()...)) {}

/*****************************************************
    MakeSwitchResult 
******************************************************/
      
    template <typename Param>
    struct ExtractSwitchParameters{
      static_assert(std::is_same<typename ExtractSwitchParameters<Param>::type, empty_t>::value, "Switch definitions can only contain case defitions");
      typedef empty_t type;
    };

    template <typename... Params>
    struct ExtractSwitchParameters<caseDef<Params...>>{
      typedef typename MakeCaseResult<Params...>::type type;
    };

    template <typename... Params>
    struct MakeSwitchResult;

    template <typename Name, typename... Params>
    struct MakeSwitchResult<Name, Params...>{

      static_assert(isName<Name>::value, "The first parameter to a swicth definition must be a name");

      typedef switchResult<Name, typename ExtractSwitchParameters<Params>::type...> type;
    };
  
/*****************************************************
    MakeCaseResult 
******************************************************/
      
    template <typename... Params>
    struct MakeCaseResult;

    template <typename Name, typename... Params>
    struct MakeCaseResult<Name, Params...>{

      static_assert(isName<Name>::value, "The first parameter to a case definition must be a name");
      
      template <typename... ParamsResult>
      static auto Makeit(std::tuple<ParamsResult...> params) -> caseResult<Name, ParamsResult...> {}

      typedef decltype(Makeit(MakeParametersResults(std::declval<Params>()...))) type;
    };

/*****************************************************
    MakeConfigResult : divide each setting and
    treat them individually
******************************************************/
      
    template <typename... Params>
    struct MakeConfigResult{
      template <typename... ParamsResult>
      static auto Makeit(std::tuple<ParamsResult...> params) -> configResult<ParamsResult...> {}

      typedef decltype(Makeit(MakeParametersResults(std::declval<Params>()...))) type;
    };
    
/*****************************************************
    MakeConfigResultWithoutName : divide each setting and
    treat them individually, avoiding settingName<> types
******************************************************/
      
    template <typename... Params>
    struct MakeConfigResultWithoutName{
      
      
    public:
      template <typename... Ps>
      static auto removeName(Ps...) -> decltype(std::tuple_cat( std::declval<typename std::conditional<isName<Ps>::value, std::tuple<>, std::tuple<Ps>>::type>()...)){
      }
      
      template <typename... ParamsResult>
      static auto Makeit(std::tuple<ParamsResult...> params) -> groupResult<ParamsResult...> {}
      
      template <typename... Ps>
      static auto callMakeConfigResult(std::tuple<Ps...>)-> decltype(Makeit(MakeParametersResults(std::declval<Ps>()...))){}
      
    public:
      typedef decltype(callMakeConfigResult(removeName(std::declval<Params>()...))) type;
    };
    
/*****************************************************
    ValidateParameters : Applies validation on all
    constraints part of the given tuple
******************************************************/
 
    template <size_t Index, size_t Size>
    struct ValidateParameters{
      template <typename Constraints, typename ResultType>
      static const bool validate( const Constraints& constraints, const ResultType& result, std::vector<ConfigError>& errors){
        typedef typename std::tuple_element<Index, Constraints>::type ConstraintType;
        bool check = ApplyConstraintCheck<ConstraintType,ResultType>::validate(std::get<Index>(constraints), result);
        if (!check){
          errors.push_back(ApplyConstraintCheck<ConstraintType,ResultType>::getError(std::get<Index>(constraints), result));
        }
        return check && ValidateParameters<Index+1,Size>::validate(constraints, result, errors);
      }
    };

    template <size_t Index>
    struct ValidateParameters<Index,Index>{
      template <typename Constraints, typename ResultType>
      static const bool validate( const Constraints& constraints, const ResultType& result, std::vector<ConfigError>& errors){
        return true;
      }
    };
   
/*****************************************************
    ApplyConstraintCheck : Applies validation on all
    constraints part of the given tuple
******************************************************/
      
    template <typename ConstraintType, typename ResultType>
    struct ApplyConstraintCheck;


    template <typename T>
    struct ApplyConstraintCheck<LengthConstraint<T>,std::string>{
      static bool validate(const LengthConstraint<T>& constraint, const std::string& result){
        return constraint.validate(result.size());
      }
      static ConfigError getError(const LengthConstraint<T>& constraint, const std::string& result){
        return constraint.getError(result.size());
      }
    };

    template <typename U, typename T>
    struct ApplyConstraintCheck<LengthConstraint<U>,std::vector<T>>{
      static bool validate(const LengthConstraint<T>& constraint, const std::string& result){
        return constraint.validate(result.size());
      }
      static ConfigError getError(const LengthConstraint<T>& constraint, const std::string& result){
        return constraint.getError(result.size());
      }
    };
   
/*****************************************************
   ExtractGroupResult : extract the values of a 
   groupResult from another groupResult
   "from" must be a superset of "to"
******************************************************/

    template <typename... ParamFrom, typename... ParamTo>
    void ExtractGroupResult(const groupResult<ParamFrom...>& pFrom, groupResult<ParamTo...>& pTo){
      char dummy[] = {CopyTupleContent(std::get<ParamTo>(pTo), std::get<ParamTo>(pTo))..., 0};
    }

    template <typename ParamFrom, typename ParamTo>
    void CopyTupleContent(const ParamFrom& pFrom, ParamTo& pTo){
      pTo = pFrom;
    }


/*****************************************************
   addressing tuple from name
******************************************************/
      
    //non-template parameter iteration
    template <typename needle, typename first_param, typename... settings>
    struct ExtractSettingFromName<needle, first_param, settings...>{
      typedef typename ExtractSettingFromName<needle, settings...>::result result;
      
      template <typename GroupType>
      static result& get(GroupType& group){
        return ExtractSettingFromName<needle, settings...>::get(group);
      }
      static const int index = ExtractSettingFromName<needle, settings...>::index+1;
    };

    //non-template terminal
    template <typename needle, typename first_param>
    struct ExtractSettingFromName<needle, first_param>{
      typedef empty_t result;
      
      template <typename GroupType>
      static result& get(GroupType& group){
        return *static_cast<empty_t*>(0);
      }
      static const int index = 0;
    };

    //non-matching key iteration
    template <typename needle, typename key, typename... first_params, template<typename...> class first_setting, typename... settings>
    struct ExtractSettingFromName<needle, first_setting<key,first_params...>, settings...>{
      typedef typename ExtractSettingFromName<needle, settings...>::result result;
      
      template <typename GroupType>
      static result& get(GroupType& group){
        return ExtractSettingFromName<needle, settings...>::get(group);
      }
      static const int index = ExtractSettingFromName<needle, settings...>::index+1;
    };

    //matching key iteration
    template <typename needle, typename... first_params, template<typename...> class first_setting, typename... nextSettings>
    struct ExtractSettingFromName<needle, first_setting<needle,first_params...>, nextSettings...>{
      typedef first_setting<needle,first_params...> result;
      
      template <typename GroupType>
      static result& get(GroupType& group){
        return std::get<first_setting<needle,first_params...>>(group.settings);
      }
      static const int index = 0;
    };

    //non-matching key terminal
    template <typename needle, typename key, typename... first_params, template<typename...> class first_setting>
    struct ExtractSettingFromName<needle, first_setting<key,first_params...>>{
      typedef empty_t result;
      
      template <typename GroupType>
      static result& get(GroupType& group){
        return *static_cast<empty_t*>(0);
      }
      static const int index = 0;
    };

    //matching key terminal
    template <typename needle, typename... first_params, template<typename...> class first_setting>
    struct ExtractSettingFromName<needle, first_setting<needle,first_params...>>{
      typedef first_setting<needle,first_params...> result;
      
      template <typename GroupType>
      static result& get(GroupType& group){
        return std::get<first_setting<needle,first_params...>>(group.settings);
      }
      static const int index = 0;
    };
      
    /*****************************************************
       counting number of settings in a parameter pack
    ******************************************************/
    template <typename... values>
    struct getSettingsCount_t{
      static const size_t value = std::tuple_size<std::tuple<values...>>::value;
    };

    /*****************************************************
       groupResult
    ******************************************************/

    template <typename... Values>
    struct groupResult{

    public:
      typedef std::tuple<Values...> myTupleType;
      std::tuple<Values...> settings;
      
      static const size_t size = std::tuple_size<myTupleType>::value;

    public:
      
      groupResult(){}

      template <typename... Params>
      groupResult(const groupResult<Params...>& pFrom){
        ExtractGroupResult(pFrom, *this);
      }

      template <typename Name>
      auto getSetting() -> typename std::add_lvalue_reference<typename ExtractSettingFromName<Name,Values...>::result>::type{
        return std::get<ExtractSettingFromName<Name,Values...>::index>(settings);
      }
      
      template <size_t index>
      auto getNthSetting() -> typename std::add_lvalue_reference<typename std::tuple_element<index, myTupleType>::type>::type{
        return std::get<index>(settings);
      }
      
      template <size_t index>
      struct getNthSettingType{
        typedef typename std::tuple_element<index, myTupleType>::type type;
      };
      
      
      template <typename Name>
      auto getValue(Name) -> typename std::add_lvalue_reference<typename ExtractSettingFromName<Name,Values...>::result::myType>::type{
        return getValue<Name>();
      }
      
      template <typename Name>
      auto getValue() -> typename std::add_lvalue_reference<typename ExtractSettingFromName<Name,Values...>::result::myType>::type{
        static_assert(!std::is_same<typename ExtractSettingFromName<Name,Values...>::result,empty_t>::value, "Setting does not exist in the configuration");
        return std::get<ExtractSettingFromName<Name,Values...>::index>(settings).getValue();
      }
      
    };

    /*****************************************************
       switchResult
    ******************************************************/


    //matching name iteration
    template <typename CaseName, typename... CaseParams, typename... OtherCases>
    struct ExtractCaseFromName<CaseName, caseResult<CaseName,CaseParams...>, OtherCases...>{
      typedef caseResult<CaseName,CaseParams...> result;
      static const int index=0;
    };

    //matching name terminal
    template <typename CaseName, typename... CaseParams>
    struct ExtractCaseFromName<CaseName, caseResult<CaseName,CaseParams...>>{
      typedef caseResult<CaseName,CaseParams...> result;
      static const int index=0;
    };

    //not-matching name iteration
    template <typename Name, typename Name1, typename... CaseParams, typename... Params>
    struct ExtractCaseFromName<Name, caseResult<Name1,CaseParams...>, Params...>{
      typedef typename ExtractCaseFromName<Name, Params...>::result result;
      static const int index=ExtractCaseFromName<Name, Params...>::index+1;
    };

    //not-matching name terminal
    template <typename Name, typename Name1, typename... CaseParams>
    struct ExtractCaseFromName<Name, caseResult<Name1,CaseParams...>>{
      typedef empty_t result;
      static const int index=0;
    };

    //  GetCaseNameFromStr takes the Params... of a switchResult 
    // and call template <T> Functor::operator() with the settingName matching the ref std::string
    template <typename... Params>
    struct GetCaseNameFromStr;

    template <>
    struct GetCaseNameFromStr<>{
      template <typename SwitchType, typename Functor>
      static void call(const std::string& ref, Functor&& fctor){}
    };

    template <uint64_t T1, uint64_t T2, uint64_t T3, uint64_t T4, typename... CaseParams, typename... Params>
    struct GetCaseNameFromStr<caseResult<settingName<T1,T2,T3,T4>, CaseParams...>, Params...>{
      template <typename SwitchType, typename Functor>
      static void call(const std::string& ref, Functor&& fctor){
        if (settingName<T1,T2,T3,T4>::getString()==ref){
          fctor.template operator()<settingName<T1,T2,T3,T4>>();
        } else {
          GetCaseNameFromStr<Params...>::template call<SwitchType,Functor>(ref,fctor);
        }
      }
    };



    template <typename T, typename... Params>
    struct MaxSize{
      static const size_t size = (sizeof(T)>=MaxSize<Params...>::size?sizeof(T):MaxSize<Params...>::size);
      static const size_t alignment = (alignof(T)>=MaxSize<Params...>::alignment?alignof(T):MaxSize<Params...>::alignment);
    };

    template <typename T>
    struct MaxSize<T>{
      static const size_t size = sizeof(T);
      static const size_t alignment = alignof(T);
    };

    template <typename RefSettingName, typename... Cases>
    struct switchResult{

    private:
      typedef RefSettingName myRefSetting;
      
      std::string activeValue;
      std::function<void(void)> destroyCurrentValue;
      
    public:
      typedef switchResult<RefSettingName,Cases...> myType;
      typedef RefSettingName myName;
      
    public:
      template<typename settingName>
      auto getCase(settingName) -> typename std::add_const<typename std::add_lvalue_reference<typename ExtractCaseFromName<settingName,Cases...>::result>::type>::type{
        if (settingName::getString()!=activeValue){
          throw std::string("bad union access : "+settingName::getString()+" whereas "+activeValue+" is active");
        }
        return *reinterpret_cast<typename ExtractCaseFromName<settingName,Cases...>::result*>(&storage);
      }
      
      template<typename settingName>
      void setCase(const settingName& name, const typename ExtractCaseFromName<settingName,Cases...>::result& value){
        defineMember(name);
        (*reinterpret_cast<typename ExtractCaseFromName<settingName,Cases...>::result*>(&storage)) = value;
      }
      
      template<typename settingName>
      void activateCase(const settingName& = settingName()){
        if (destroyCurrentValue){destroyCurrentValue();}
        typedef typename ExtractCaseFromName<settingName,Cases...>::result resultType;
        resultType* res = new(&storage) resultType();
        destroyCurrentValue = [res](){delete res;};
        activeValue = settingName::getString();
      }
      
      myType& getValue(){
        return *this;
      }
      
      const myType& getValue() const {
        return *this;
      }
      
      operator std::string(){
        return activeValue;
      }
      
      template <typename Visitor>
      void visitCase(const std::string& name, Visitor& visitor){
        GetCaseNameFromStr<Cases...>::template call<myType>(name, visitor);
      }
      
    private:
      struct ActivateCase{
        myType& mMe;
        
        ActivateCase(myType& me)
        : mMe(me){}
        
        template <typename Name>
        void operator()(){
          mMe.template activateCase<Name>(Name());
        }
      };
    public:
      void activateCase(const std::string& name){
        ActivateCase define(*this);
        GetCaseNameFromStr<Cases...>::template call<myType>(name, define);
      }
      
      typename std::aligned_storage<MaxSize<Cases...>::size, MaxSize<Cases...>::alignment>::type storage;
      
    };


    /*****************************************************
       caseResult
    ******************************************************/

    template <typename CaseName, typename... Settings>
    struct caseResult{
      typedef CaseName myName;
      groupResult<Settings...> result;
      
      template <typename SettingName>
      auto getValue(SettingName) -> decltype(result.template getValue<SettingName>()){
        return result.getValue<SettingName>();
      }
      
      template <typename SettingName>
      auto getSetting(SettingName) -> decltype(result.template getSetting<SettingName>()){
        return result.getSetting<SettingName>();
      }
    };

    /*****************************************************
       configResult
    ******************************************************/

    template <typename... Params>
    struct configResult{

      template <typename...>
      friend struct configDef;
      
      template <typename>
      friend struct ConfigValueLoader_t;
      
    private:
      groupResult<Params...> result;
      std::vector<ConfigError> errors;

    public:
      
      configResult(){}
      
      template <typename... T>
      configResult(const configResult<T...>& pRhs)
      : result(pRhs.result){
      
      }
      
      template <typename... T>
      configResult(const caseResult<T...>& pRhs)
      : result(pRhs.result){
      
      }
      
      template <typename... T>
      configResult(const groupResult<T...>& pRhs)
      : result(pRhs){
      
      }
      
      template <typename Name>
      auto getValue(Name) -> decltype(result.template getValue<Name>()){
        if (hasError()){
          throw std::string("Cannot access  config when error occured");
        }
        return result.getValue<Name>();
      }
      
      template <typename Name>
      auto getSetting(Name) -> decltype(result.template getSetting<Name>()){
        if (hasError()){
          throw std::string("Cannot access  config when error occured");
        }
        return result.getSetting<Name>();
      }
      
      bool hasError() const{
        return errors.size()>0;
      }
      
      const std::vector<ConfigError>& getErrors(){
        return errors;
      }
      
    private:
      void setErrors(const std::vector<ConfigError>& err){
        errors = err;
      }
    };

    /*****************************************************
       settingResult
    ******************************************************/

    template <typename Name, typename type>
    struct settingResult{
      typedef Name myName;
      typedef type myType;
      
      myType value;
      
      settingResult(){}
      
      settingResult(myType val)
      : value(val){}
      
      myType& getValue(){
        return value;
      }
      
      const myType& getValue() const {
        return value;
      }
    };

    /*****************************************************
       Loader
    ******************************************************/

    //interates over a group of setting
    template <size_t index, size_t size>
    struct ConfigGroupLoader_t{

      template <typename ConfigBackend, typename... DefParams, typename... ResParams>
      static std::vector<ConfigError> load(const std::tuple<DefParams...>& definition, groupResult<ResParams...>& result, const ConfigBackend& configLoader, const typename ConfigBackend::ValueType& parentValue){
        auto& resElm = result.template getNthSetting<index>();
        const auto& defElm = std::get<ExtractSettingFromName<typename std::decay<decltype(resElm)>::type::myName, DefParams...>::index>(definition);
        std::string settingName = std::decay<decltype(resElm)>::type::myName::getString();
        std::vector<ConfigError> errors = ConfigValueLoader_t<ConfigBackend>::load(configLoader, parentValue, settingName, resElm, defElm);
        std::vector<ConfigError> nextErrors = ConfigGroupLoader_t<index+1,size>::load(definition, result, configLoader, parentValue);
        errors.insert(errors.end(), nextErrors.begin(), nextErrors.end());
        return errors;
      }
    };

    template <size_t index>
    struct ConfigGroupLoader_t<index,index>{
      template <typename ConfigBackend, typename... DefParams, typename... ResParams>
      static std::vector<ConfigError> load(const std::tuple<DefParams...>& definition,groupResult<ResParams...>& config, const ConfigBackend&, const typename ConfigBackend::ValueType&){
        return std::vector<ConfigError>();
      }
    };

    template <typename ConfigBackend, typename SwitchType, typename SettingName, typename... DefParams>
    struct caseResultLoaderVisitor{

      ConfigBackend& mConfigLoader;
      const typename ConfigBackend::ValueType& mParentValue;
      SwitchType& mSwitchRes;
      const switchDef<SettingName, DefParams...>& mDefinition;
      
      std::vector<ConfigError> mErrors;

      caseResultLoaderVisitor(ConfigBackend& configLoader, const typename ConfigBackend::ValueType& parentValue, SwitchType& switchRes, const switchDef<SettingName, DefParams...>& definition)
      : mConfigLoader(configLoader)
      , mParentValue(parentValue)
      , mSwitchRes(switchRes)
      , mDefinition(definition){}

      template <typename Name>
      void operator()(){
        mErrors = ConfigValueLoader_t<ConfigBackend>::load(mConfigLoader, mParentValue, mSwitchRes.template getCase(Name()), mDefinition.template getCase(Name()));
      }
    };

    template <typename ConfigBackend, typename SwitchType, typename SettingName, typename... DefParams>
    caseResultLoaderVisitor<ConfigBackend,SwitchType,SettingName,DefParams...> make_caseResultLoaderVisitor(ConfigBackend& configLoader, const typename ConfigBackend::ValueType& parentValue, SwitchType& switchRes, const switchDef<SettingName, DefParams...>& definition){
      return caseResultLoaderVisitor<ConfigBackend,SwitchType,SettingName,DefParams...>(configLoader, parentValue, switchRes, definition);
    }

    template <typename ConfigBackend>
    struct ConfigValueLoader_t{

      //load config
      template<typename... DefParams, typename... ResParams>
      static std::vector<ConfigError> load(const ConfigBackend& configLoader, configResult<ResParams...>& setting, const configDef<DefParams...>& definition){
        std::vector<ConfigError> errors = ConfigGroupLoader_t<0,groupResult<ResParams...>::size>::load(definition.content, setting.result, configLoader, configLoader.getRoot());
        std::vector<ConfigError> groupErrors = definition.validate(setting);
        errors.insert(errors.end(), groupErrors.begin(), groupErrors.end());
        return errors;
        
      }
      
      //load setting
      template <typename SettingResType, typename SettingName, typename... DefParams>
      static std::vector<ConfigError> load(const ConfigBackend& configLoader, const typename ConfigBackend::ValueType& parentValue, const std::string& settingName,  SettingResType& setting, const settingDef<SettingName, DefParams...>& definition){
        if (configLoader.hasValue(parentValue, settingName)){
          configLoader.getValue(parentValue, settingName, setting.value);
          return definition.validate(setting);
        } else {
          return std::vector<ConfigError>({ConfigError("The required setting "+settingName+" wasn't found in the configuration")});
        }
        
      }
      
      //load section
      template<typename SettingName, typename... DefParams, typename... ResParams>
      static std::vector<ConfigError> load(const ConfigBackend& configLoader, const typename ConfigBackend::ValueType& parentValue, const std::string& settingName, settingResult<SettingName, groupResult<ResParams...>>& setting, const sectionDef<DefParams...>& definition){
        
        if (configLoader.hasValue(parentValue, settingName)){
          ConfigGroupLoader_t<0,groupResult<ResParams...>::size>::load(definition.content, setting.value, configLoader, configLoader.getContainedValue(parentValue, settingName));
          return definition.validate(setting);
        } else {
          return std::vector<ConfigError>({ConfigError("The required setting "+settingName+" wasn't found in the configuration")});
        }
      }
      
      //load map
      template<typename SettingName, typename... DefParams, typename... ResParams>
      static std::vector<ConfigError> load(const ConfigBackend& configLoader, const typename ConfigBackend::ValueType& parentValue, const std::string& settingName, settingResult<SettingName, std::unordered_map<std::string,groupResult<ResParams...>>>& setting, const mapDef<DefParams...>& definition){
        
        if (configLoader.hasValue(parentValue, settingName)){
          std::vector<ConfigError> errors;
          typename ConfigBackend::ValueType mapConfigVal = configLoader.getContainedValue(parentValue, settingName);
          std::vector<std::string> values = configLoader.getContainedValues(mapConfigVal);
          for (auto& valueName : values){
            typename ConfigBackend::ValueType configVal = configLoader.getContainedValue(mapConfigVal, valueName);
            groupResult<ResParams...> result;
            std::vector<ConfigError> groupLoadErrors = ConfigGroupLoader_t<0,groupResult<ResParams...>::size>::load(definition.content, result, configLoader, configVal);
            setting.value.insert(std::make_pair(valueName, result));
            errors.insert(errors.end(), groupLoadErrors.begin(), groupLoadErrors.end());
          }
          std::vector<ConfigError> validationErrors = definition.validate(setting);
          errors.insert(errors.end(), validationErrors.begin(), validationErrors.end());
          return errors;
        } else {
          return std::vector<ConfigError>({ConfigError("The required setting "+settingName+" wasn't found in the configuration")});
        }
      }
      
      //load switch
      template <typename SettingResType, typename SettingName, typename... DefParams>
      static std::vector<ConfigError> load(const ConfigBackend& configLoader, const typename ConfigBackend::ValueType& parentValue, const std::string& settingName, SettingResType& setting, const switchDef<SettingName, DefParams...>& definition){
        if (configLoader.hasValue(parentValue, settingName)){
          std::string switchValue;
          configLoader.getValue(parentValue, settingName, switchValue);
          setting.activateCase(switchValue);
          
          auto visitor = make_caseResultLoaderVisitor(configLoader, parentValue, setting, definition);
          setting.template visitCase(switchValue, visitor);
          std::vector<ConfigError> validationErrors = definition.validate(setting);
          visitor.mErrors.insert(visitor.mErrors.end(), validationErrors.begin(), validationErrors.end());
          return visitor.mErrors;
        } else {
          return std::vector<ConfigError>({ConfigError("The required setting "+settingName+" wasn't found in the configuration")});
        }
        
      }
      
        //load case
      template <typename SettingResType, typename SettingName, typename... DefParams>
      static std::vector<ConfigError> load(const ConfigBackend& configLoader, const typename ConfigBackend::ValueType& parentValue, SettingResType& setting, const caseDef<SettingName, DefParams...>& definition){
        std::vector<ConfigError> errors = ConfigGroupLoader_t<0,decltype(setting.result)::size>::load(definition.content, setting.result, configLoader, parentValue);
        std::vector<ConfigError> groupErrors = definition.validate(setting);
        errors.insert(errors.end(), groupErrors.begin(), groupErrors.end());
        return errors;
      }
      
      //load optional setting
      template <typename SettingResType, typename SettingName, typename... DefParams>
      static std::vector<ConfigError> load(const ConfigBackend& configLoader, const typename ConfigBackend::ValueType& parentValue, const std::string& settingName,  std::optional<SettingResType>& setting, const settingDef<SettingName, DefParams...>& definition){
        if (configLoader.hasValue(parentValue, settingName)){
          configLoader.getValue(parentValue, settingName, setting);
          return definition.validate(setting);
        } else {
          //nothing for optional
          return std::vector<ConfigError>();
        }
        
      }
    };

  }
}