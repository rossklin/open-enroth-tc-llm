cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

set(LLM_SOURCES
    compileLLMContext.cpp
    google-api.cpp
    GMTools.cpp
)

set(LLM_HEADERS
    compileLLMContext.h
    google-api.h
    GMState.h
    GMTools.h
)

add_library(llm STATIC ${LLM_SOURCES} ${LLM_HEADERS})
target_check_style(llm)

target_link_libraries(llm PUBLIC utility CURL::libcurl)

