#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <dirent.h>
#include <algorithm>
#include <unistd.h>

#define WIDTH 800
#define HEIGHT 600
 
typedef struct entry {
    std::string path;
    std::string type;
    
    //textures
    SDL_Texture *file_name_t;
    SDL_Texture *icon_texture_t;
    SDL_Texture *file_size_t;
    SDL_Texture *file_perm_t;
    SDL_Texture *click_box_t;

    //rects
    SDL_Rect file_name_r;
    SDL_Rect icon_texture_r;
    SDL_Rect file_size_r;
    SDL_Rect file_perm_r;
    SDL_Rect click_box_r;
} entry;
 
typedef struct AppData {

    std::vector<entry> entries;

    TTF_Font *font;
    SDL_Texture *directory;
    SDL_Texture *executable;
    SDL_Texture *image;
    SDL_Texture *video;
    SDL_Texture *code;
    SDL_Texture *other;
    SDL_Texture *arrow;
    SDL_Texture *file_name_header_t;
    SDL_Texture *file_size_header_t;
    SDL_Texture *file_perms_header_t;
    SDL_Rect file_name_header_r;
    SDL_Rect file_size_header_r;
    SDL_Rect file_perms_header_r;
    SDL_Rect top_bar;
    bool item_selected;
    // SDL_Point offset;
} AppData;
 
void initialize(SDL_Renderer *renderer, AppData *data_ptr);
void render(SDL_Renderer *renderer, AppData *data_ptr);
void quit(AppData *data_ptr);
std::vector<entry> initializeEntries(std::string dirPath, AppData *data_ptr, SDL_Renderer *renderer);
std::string getFileType(std::string fileName);
std::string getFilePerms(std::string fileName);

void splitString(std::string text, char d, std::vector<std::string>& result);
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result);
std::string getFileSize(std::string path);

int main(int argc, char **argv){
    char *home = getenv("HOME");
    printf("HOME: %s\n", home);    

    // initializing SDL as Video
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    // create window and renderer
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
 
    // initialize and perform rendering loop
    AppData data;
    initialize(renderer, &data);

    data.entries = initializeEntries(home, &data, renderer);
    data.item_selected = false;
    render(renderer, &data);
    SDL_Event event;
    SDL_WaitEvent(&event);
    while (event.type != SDL_QUIT)
    {
        SDL_WaitEvent(&event);
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN:
                for (int i = 0; i < data.entries.size(); i++){
                    if (data.item_selected == false && event.button.button == SDL_BUTTON_LEFT &&
                    (event.button.x >= data.entries[i].click_box_r.x &&
                    event.button.x <= data.entries[i].click_box_r.x + data.entries[i].click_box_r.w &&
                    event.button.y >= data.entries[i].click_box_r.y &&
                    event.button.y <= data.entries[i].click_box_r.y + data.entries[i].click_box_r.h))
                    {   
                        data.item_selected = true;
                        if (data.entries[i].type == "Directory" || data.entries[i].type == ".."){
                            data.entries = initializeEntries(data.entries[i].path, &data, renderer);
                        } else {
                            int pid;
                            pid = fork();
                            if (pid == 0){
                                std::string xdgPath = "/user/bin/xdg-open";
                                std::vector<std::string> fileArgsVector = {xdgPath,data.entries[i].path};
                                char** fileArgs;
                                vectorOfStringsToArrayOfCharArrays(fileArgsVector, &fileArgs);
                                execv("/usr/bin/xdg-open", fileArgs);
                            }
                        }
                    }
                }

                break;
            case SDL_MOUSEBUTTONUP:
                data.item_selected = false;
                break;  
        }
 
        render(renderer, &data);
    }
 
    // clean up
    quit(&data);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
 
 
    return 0;
}
 
void initialize(SDL_Renderer *renderer, AppData *data_ptr)
{

    // SDL_Color color = {.r = 255, .g = 255, .b = 255}; //init screen to white
    data_ptr->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 24);


    SDL_Surface *directorySurface = IMG_Load("resrc/images/directory.png");
    data_ptr->directory = SDL_CreateTextureFromSurface(renderer, directorySurface);
    SDL_FreeSurface(directorySurface);

    SDL_Surface *executableSurface = IMG_Load("resrc/images/executable.png");
    data_ptr->executable = SDL_CreateTextureFromSurface(renderer, executableSurface);
    SDL_FreeSurface(executableSurface);

    SDL_Surface *imageSurface = IMG_Load("resrc/images/image.png");
    data_ptr->image = SDL_CreateTextureFromSurface(renderer, imageSurface);
    SDL_FreeSurface(imageSurface);

    SDL_Surface *videoSurface = IMG_Load("resrc/images/video.png");
    data_ptr->video = SDL_CreateTextureFromSurface(renderer, videoSurface);
    SDL_FreeSurface(videoSurface);

    SDL_Surface *codeSurface = IMG_Load("resrc/images/code.png");
    data_ptr->code = SDL_CreateTextureFromSurface(renderer, codeSurface);
    SDL_FreeSurface(codeSurface);

    SDL_Surface *otherSurface = IMG_Load("resrc/images/other.png");
    data_ptr->other = SDL_CreateTextureFromSurface(renderer, otherSurface);
    SDL_FreeSurface(otherSurface);

    SDL_Surface *arrowSurface = IMG_Load("resrc/images/arrow.png");
    data_ptr->arrow = SDL_CreateTextureFromSurface(renderer, arrowSurface);
    SDL_FreeSurface(arrowSurface);

 
    data_ptr->top_bar.x = 0;
    data_ptr->top_bar.y = 0;
    data_ptr->top_bar.w = WIDTH;
    data_ptr->top_bar.h = 30;


    // name header
    SDL_Color color = { 0, 0, 0 };
    SDL_Surface *phrase_surf_name = TTF_RenderText_Solid(data_ptr->font, "Name", color);
    data_ptr->file_name_header_t = SDL_CreateTextureFromSurface(renderer, phrase_surf_name);
    SDL_FreeSurface(phrase_surf_name);
    data_ptr->file_name_header_r.x = 70;
    data_ptr->file_name_header_r.y = 0;
    SDL_QueryTexture(data_ptr->file_name_header_t, NULL, NULL, &(data_ptr->file_name_header_r.w), &(data_ptr->file_name_header_r.h));

    // size header
    SDL_Surface *phrase_surf_size = TTF_RenderText_Solid(data_ptr->font, "Size", color);
    data_ptr->file_size_header_t = SDL_CreateTextureFromSurface(renderer, phrase_surf_size);
    SDL_FreeSurface(phrase_surf_size);
    data_ptr->file_size_header_r.x = 550;
    data_ptr->file_size_header_r.y = 0;
    SDL_QueryTexture(data_ptr->file_size_header_t, NULL, NULL, &(data_ptr->file_size_header_r.w), &(data_ptr->file_size_header_r.h));

    // perms header
    SDL_Surface *phrase_surf_perms = TTF_RenderText_Solid(data_ptr->font, "Perms", color);
    data_ptr->file_perms_header_t = SDL_CreateTextureFromSurface(renderer, phrase_surf_perms);
    SDL_FreeSurface(phrase_surf_perms);
    data_ptr->file_perms_header_r.x = 670;
    data_ptr->file_perms_header_r.y = 0;
    SDL_QueryTexture(data_ptr->file_perms_header_t, NULL, NULL, &(data_ptr->file_perms_header_r.w), &(data_ptr->file_perms_header_r.h));
}
 
void render(SDL_Renderer *renderer, AppData *data_ptr)
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderClear(renderer);
     
    // TODO: draw!
    SDL_SetRenderDrawColor( renderer, 115, 115, 115, 255 );

    SDL_RenderFillRect( renderer, &data_ptr->top_bar);

    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);

    SDL_RenderCopy(renderer, data_ptr->file_name_header_t, NULL, &(data_ptr->file_name_header_r));
    SDL_RenderCopy(renderer, data_ptr->file_size_header_t, NULL, &(data_ptr->file_size_header_r));\
    SDL_RenderCopy(renderer, data_ptr->file_perms_header_t, NULL, &(data_ptr->file_perms_header_r));

    for (int i = 0; i < data_ptr->entries.size(); i++){
        if (data_ptr->entries[i].type != ".."){
            SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
            SDL_RenderFillRect( renderer, &data_ptr->entries[i].click_box_r);
            SDL_RenderCopy(renderer, data_ptr->entries[i].file_perm_t, NULL, &(data_ptr->entries[i].file_perm_r));
            SDL_RenderCopy(renderer, data_ptr->entries[i].file_size_t, NULL, &(data_ptr->entries[i].file_size_r));
            SDL_RenderCopy(renderer, data_ptr->entries[i].file_name_t, NULL, &(data_ptr->entries[i].file_name_r));
        }
        SDL_RenderCopy(renderer, data_ptr->entries[i].icon_texture_t, NULL, &(data_ptr->entries[i].icon_texture_r));
    }
 
    // show rendered frame
    SDL_RenderPresent(renderer);
}
 
void quit(AppData *data_ptr)
{
    SDL_DestroyTexture(data_ptr->directory);
    SDL_DestroyTexture(data_ptr->executable);
    SDL_DestroyTexture(data_ptr->image);
    SDL_DestroyTexture(data_ptr->video);
    SDL_DestroyTexture(data_ptr->directory);
    SDL_DestroyTexture(data_ptr->code);
    SDL_DestroyTexture(data_ptr->other);
    SDL_DestroyTexture(data_ptr->arrow);
    SDL_DestroyTexture(data_ptr->file_name_header_t);
    SDL_DestroyTexture(data_ptr->file_size_header_t);
    SDL_DestroyTexture(data_ptr->file_perms_header_t);   

    TTF_CloseFont(data_ptr->font);
}


std::vector<entry> initializeEntries(std::string dirPath, AppData *data_ptr, SDL_Renderer *renderer){

    std::vector<entry> directoryEntries;

    std::vector<std::string> filesInDir;

    DIR* dir;
    struct dirent *entryInDir;
    std::string entryString;
    dir = opendir(dirPath.c_str());

    if (dir){
        while ((entryInDir = readdir(dir)) != NULL){
            entryString = entryInDir->d_name;
            if (entryString[0] == '.'){
                if (entryString == ".."){
                    filesInDir.push_back(entryString);
                }
            } else {
                filesInDir.push_back(entryString);
            }
        }
        closedir(dir);
    } else {
        std::cout << "ERROR: could not open directory\n";
        return directoryEntries;
    }

    std::sort(filesInDir.begin(), filesInDir.end());

    std::string filePath;
    std::string fileName;
    struct stat file;

    entry entry;

    int windowSize = 570;
    int barOffset = 30;

    int startXPosition = 10;
    int startYPosition = 10;

    int indexEntry = 0;

    for (int i = 0; i < filesInDir.size(); i++){

        fileName = filesInDir[i];
        filePath = dirPath + "/" + filesInDir[i];
        stat(filePath.c_str(), &file);

        if (fileName != ".."){
            if (S_ISDIR(file.st_mode)){
                entry.type = "Directory";
                entry.path = filePath;
                entry.icon_texture_t = data_ptr->directory;
            } else if (file.st_mode & S_IXUSR){
                entry.type = "Executable";
                entry.path = filePath;
                entry.icon_texture_t = data_ptr->executable;
            } else if (getFileType(fileName) == std::string("Image")){
                entry.type = "Image";
                entry.path = filePath;
                entry.icon_texture_t = data_ptr->image;
            } else if (getFileType(fileName) == std::string("Video")){
                entry.type = getFileType(fileName);
                entry.path = filePath;
                entry.icon_texture_t = data_ptr->video;
            } else if (getFileType(fileName) == std::string("Code file")){
                entry.type = getFileType(fileName);
                entry.path = filePath;
                entry.icon_texture_t = data_ptr->code;
            } else {
                entry.type = getFileType(fileName);
                entry.path = filePath;
                entry.icon_texture_t = data_ptr->other;
            }
            // get rid of divide by zero errors below
            int size = filesInDir.size() - 1;
            if (size < 11){
                size = 11;
            }

            entry.icon_texture_r.x = startXPosition;
            entry.icon_texture_r.y = (int)((indexEntry * windowSize/size) + barOffset);
            entry.icon_texture_r.h = (int)(windowSize/size);
            entry.icon_texture_r.w = (int)(windowSize/size);
            SDL_Color color = { 0, 0, 0 };
            SDL_Surface *phrase_surf_name = TTF_RenderText_Solid(data_ptr->font, fileName.c_str(), color);
            entry.file_name_t = SDL_CreateTextureFromSurface(renderer, phrase_surf_name);
            SDL_FreeSurface(phrase_surf_name);
            SDL_QueryTexture(entry.file_name_t, NULL, NULL, &(entry.file_name_r.w), &(entry.file_name_r.h));
            entry.file_name_r.w = (int)(entry.file_name_r.w / (size / 11));
            entry.file_name_r.h = (int)(entry.file_name_r.h / (size / 11));
            entry.file_name_r.x = (int)(startXPosition + (windowSize/size) + windowSize/size/2);
            entry.file_name_r.y = (int)(((indexEntry * windowSize/size)) + (windowSize/size/6) + barOffset);

            entry.click_box_r.x = 0;
            entry.click_box_r.y = (int)((indexEntry * windowSize/size) + barOffset);
            entry.click_box_r.h = (int)(windowSize/size) - 2;
            entry.click_box_r.w = 800;

            SDL_Surface *phrase_surf_size = TTF_RenderText_Solid(data_ptr->font, getFileSize(filePath).c_str(), color);
            entry.file_size_t = SDL_CreateTextureFromSurface(renderer, phrase_surf_size);
            SDL_FreeSurface(phrase_surf_size);
            SDL_QueryTexture(entry.file_size_t, NULL, NULL, &(entry.file_size_r.w), &(entry.file_size_r.h));
            entry.file_size_r.w = (int)(entry.file_size_r.w / (size / 11));
            entry.file_size_r.h = (int)(entry.file_size_r.h / (size / 11));
            entry.file_size_r.x = 550;
            entry.file_size_r.y = (int)(((indexEntry * windowSize/size)) + (windowSize/size/6) + barOffset);

            SDL_Surface *phrase_surf_perm = TTF_RenderText_Solid(data_ptr->font, getFilePerms(filePath).c_str(), color);
            entry.file_perm_t = SDL_CreateTextureFromSurface(renderer, phrase_surf_perm);
            SDL_FreeSurface(phrase_surf_perm);
            SDL_QueryTexture(entry.file_perm_t, NULL, NULL, &(entry.file_perm_r.w), &(entry.file_perm_r.h));
            entry.file_perm_r.w = (int)(entry.file_perm_r.w / (size / 11));
            entry.file_perm_r.h = (int)(entry.file_perm_r.h / (size / 11));
            entry.file_perm_r.x = 660;
            entry.file_perm_r.y = (int)(((indexEntry * windowSize/size)) + (windowSize/size/6) + barOffset);



            directoryEntries.push_back(entry);
            indexEntry = indexEntry + 1;
        } else {
            entry.type = "..";
            entry.path = filePath;
            entry.icon_texture_t = data_ptr->arrow;
            entry.icon_texture_r.x = 10;
            entry.icon_texture_r.y = 0;
            entry.icon_texture_r.h = 28;
            entry.icon_texture_r.w = 28;

            entry.click_box_r.x = 10;
            entry.click_box_r.y = 0;
            entry.click_box_r.h = 28;
            entry.click_box_r.w = 28;


            directoryEntries.push_back(entry);
        }
    }
    return directoryEntries;
}

std::string getFileType(std::string fileName){
    if (fileName.c_str() != ".."){
        int dotIndex = fileName.find_first_of('.');
        if (dotIndex > 0 && dotIndex < fileName.size() - 1){
            std::string fileExt = fileName.substr(dotIndex, fileName.size() - 1);
            if (fileExt == ".jpg" || fileExt == ".jpeg" || fileExt == ".png" || fileExt == ".tif" || fileExt == ".tiff" || fileExt == ".gif"){
                return std::string("Image");
            } else if (fileExt == ".mp3" || fileExt == ".mp4" || fileExt == ".mov" || fileExt == ".mkv" || fileExt == ".avi" || fileExt == ".webm"){
                return std::string("Video");
            } else if (fileExt == ".h" || fileExt == ".c" || fileExt == ".cpp" || fileExt == ".py" || fileExt == ".java" || fileExt == ".js"){
                return std::string("Code file");
            } else {
                return std::string("Other");
            }
        } else {
            return std::string("ERROR: file extension error");
        }
    } else {
        return std::string("ERROR: file extension error");
    }
}

void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result){
    int i;
    int result_length = list.size() + 1;
    *result = new char*[result_length];
    for (i = 0; i < list.size(); i++)
    {
        (*result)[i] = new char[list[i].length() + 1];
        strcpy((*result)[i], list[i].c_str());
    }
    (*result)[list.size()] = NULL;
}

std::string getFileSize(std::string path){
    struct stat file;
    stat(path.c_str(), &file);

    size_t size = file.st_size;
    std::string sizeString;
    if (size > 1073741824){
        size = size / 1073741824;
        sizeString = std::to_string(size) + " GiB";
    } else if (size > 10480576){
        size = size / 10480576;
        sizeString = std::to_string(size) + " MiB";
    } else if (size > 1024){
        size = size / 1024;
        sizeString = std::to_string(size) + " KiB";
    } else {
        sizeString = std::to_string(size) + " B";
    }
    return sizeString;


}

std::string getFilePerms(std::string path){
    struct stat file;
    stat(path.c_str(), &file);
    std::string permString;

    if (file.st_mode & S_IRUSR){
        permString = permString + "r";
    } else {
        permString = permString + "-";
    }

    if (file.st_mode & S_IWUSR){
        permString = permString + "w";
    } else {
        permString = permString + "-";
    }

    if (file.st_mode & S_IXUSR){
        permString = permString + "x";
    } else {
        permString = permString + "-";
    }

    permString = permString + "-";

    if (file.st_mode & S_IRGRP){
        permString = permString + "r";
    } else {
        permString = permString + "-";
    }

    if (file.st_mode & S_IWGRP){
        permString = permString + "w";
    } else {
        permString = permString + "-";
    }

    if (file.st_mode & S_IXGRP){
        permString = permString + "x";
    } else {
        permString = permString + "-";
    }

    permString = permString + "-";

    if (file.st_mode & S_IROTH){
        permString = permString + "r";
    } else {
        permString = permString + "-";
    }

    if (file.st_mode & S_IWOTH){
        permString = permString + "w";
    } else {
        permString = permString + "-";
    }

    if (file.st_mode & S_IXOTH){
        permString = permString + "x";
    } else {
        permString = permString + "-";
    }
    return permString;
}



void splitString(std::string text, char d, std::vector<std::string>& result){
    enum states { NONE, IN_WORD, IN_STRING } state = NONE;

    int i;
    std::string token;
    result.clear();
    for (i = 0; i < text.length(); i++)
    {
        char c = text[i];
        switch (state) {
            case NONE:
                if (c != d)
                {
                    if (c == '\"')
                    {
                        state = IN_STRING;
                        token = "";
                    }
                    else
                    {
                        state = IN_WORD;
                        token = c;
                    }
                }
                break;
            case IN_WORD:
                if (c == d)
                {
                    result.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
            case IN_STRING:
                if (c == '\"')
                {
                    result.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
        }
    }
    if (state != NONE)
    {
        result.push_back(token);
    }
}

