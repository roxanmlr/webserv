NAME		= webserv
CXX			= c++ -std=c++98
#DEPFLAGS    = -MMD -MP
#SAN			= -fsanitize=address,undefined
#SAN			= -fsanitize=address
CXXFLAGS	= -Wall -Wextra -Werror $(SAN) -g $(DEPFLAGS)
# To update sources
# make us
SOURCES = src/main.cpp src/cgi/CgiHandler.cpp src/config/Config.cpp src/config/ConfigError.cpp src/config/ConfigParser.cpp src/config/IConfig.cpp src/config/LocationConfigBuilder.cpp src/config/LocationConfig.cpp src/config/ServerConfigBuilder.cpp src/config/ServerConfig.cpp src/config/Tokenizer.cpp src/handler/DeleteHandler.cpp src/handler/RedirectionHandler.cpp src/handler/StaticFileHandler.cpp src/handler/UploadHandler.cpp src/http/HttpRequest.cpp src/http/HttpResponse.cpp src/server/Client.cpp src/server/WebServer.cpp src/server/WebServerError.cpp src/utils/ft_itoa.cpp src/utils/Logger.cpp src/utils/set_nonblocking.cpp
OBJECTS = $(patsubst src/%.cpp, objs/%.o,$(SOURCES))

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^
objs/%.o:src/%.cpp
	mkdir  -p "$$(dirname $@)"
	$(CXX) $(CXXFLAGS) -c -o $@ $^
clean:
	rm -rf objs/
	rm -f update-sources
fclean: clean
	rm -rf $(NAME)
re: fclean all
.PHONY : all clean fclean re

format: 
	find . \( -name '*.hpp' -o -name '*.cpp' -o -name '*.tpp' \) -exec clang-format -i {} +
#f: format
#update-sources: $(wildcard src/*/*.cpp)
#	@sed -i 's|^SOURCES.*|SOURCES = $(filter-out %/test% test%, $(wildcard *.cpp  */*.cpp  */*/*.cpp ))|' Makefile
	#touch update-sources
#
#us: update-sources
#
-include $(DEPS)
