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

    //rects
    SDL_Rect file_name_r;
    SDL_Rect icon_texture_r;
    SDL_Rect file_size_r;
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
    SDL_Texture *penguin;
    SDL_Texture *phrase;
    SDL_Rect penguin_rect;
    SDL_Rect phrase_rect;
    bool penguin_selected;
    bool phrase_selected;
    SDL_Point offset;
} AppData;
 
void initialize(SDL_Renderer *renderer, AppData *data_ptr);
void render(SDL_Renderer *renderer, AppData *data_ptr);
void quit(AppData *data_ptr);
std::vector<entry> initializeEntries(std::string dirPath, AppData *data_ptr, SDL_Renderer *renderer);
std::string getFileType(std::string fileName);

void splitString(std::string text, char d, std::vector<std::string>& result);
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result);

int main(int argc, char **argv){
    char *home = getenv("HOME");
    printf("HOME: %s\n", home);    
    
    // int pid;
    // pid = fork();
    // if (pid == 0){
    //     std::string xdgPath = "/user/bin/xdg-open";
    //     std::vector<std::string> fileArgsVector = {xdgPath,"/home/hyim/test.png"};
    //     char** fileArgs;
    //     vectorOfStringsToArrayOfCharArrays(fileArgsVector, &fileArgs);
    //     execv("/usr/bin/xdg-open", fileArgs);
    // }

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

    render(renderer, &data);
    SDL_Event event;
    SDL_WaitEvent(&event);
    while (event.type != SDL_QUIT)
    {
        SDL_WaitEvent(&event);
        switch (event.type) {
    //         case SDL_MOUSEMOTION:
    //             if (data.penguin_selected)
    //             {
    //                 data.penguin_rect.x = event.motion.x - data.offset.x;
    //                 data.penguin_rect.y = event.motion.y - data.offset.y;
    //             }
    //             else if (data.phrase_selected)
    //             {
    //                 data.phrase_rect.x = event.motion.x - data.offset.x;
    //                 data.phrase_rect.y = event.motion.y - data.offset.y;
    //             }
    //             break;
            // case SDL_MOUSEBUTTONDOWN:
            //     if (event.button.button == SDL_BUTTON_LEFT &&
            //         event.button.x >= data.phrase_rect.x &&
            //         event.button.x <= data.phrase_rect.x + data.phrase_rect.w &&
            //         event.button.y >= data.phrase_rect.y &&
            //         event.button.y <= data.phrase_rect.y + data.phrase_rect.h)
            //     {
            //         // data.phrase_selected = true;
            //         // data.offset.x = event.button.x - data.phrase_rect.x;
            //         // data.offset.y = event.button.y - data.phrase_rect.y;
            //     }
            //     else if (event.button.button == SDL_BUTTON_LEFT &&
            //         event.button.x >= data.directory_rect.x &&
            //         event.button.x <= data.directory_rect.x + data.directory_rect.w &&
            //         event.button.y >= data.directory_rect.y &&
            //         event.button.y <= data.directory_rect.y + data.directory_rect.h)
            //     {
                    // std::vector<std::string> test = getDirectoryContentInfo(std::string("/home/hyim/Documents"));


                    // std::vector<std::string> entry;



                    // //figure out what should be printed here

                    // // prints FileType | FileName | FilePath | IconPath
                    // for (int i = 0; i < test.size(); i++){
                    //     splitString(test[i], '|', entry);
                    //     for (int j = 0; j < entry.size(); j++){
                    //         std::cout << entry[j] << " | ";
                    //     }
                    //     std::cout << "\n";
                    // }
                    // // data.penguin_selected = true;
                    // // data.offset.x = event.button.x - data.penguin_rect.x;
                    // // data.offset.y = event.button.y - data.penguin_rect.y;
                // }
                //break;
    //         case SDL_MOUSEBUTTONUP:
    //             data.phrase_selected = false;
    //             data.penguin_selected = false;
    //             break;  
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
    data_ptr->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 20);


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


    // data_ptr->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 24);
 
    // SDL_Surface *img_surf = IMG_Load("resrc/images/linux-penguin.png");
    // data_ptr->penguin = SDL_CreateTextureFromSurface(renderer, img_surf);
    // SDL_FreeSurface(img_surf);
    // data_ptr->penguin_rect.x = 200;
    // data_ptr->penguin_rect.y = 100;
    // data_ptr->penguin_rect.w = 165;
    // data_ptr->penguin_rect.h = 200;
    // data_ptr->penguin_selected = false;
 
    // SDL_Color color = { 0, 0, 0 };
    // SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, "Hello World!", color);
    // data_ptr->phrase = SDL_CreateTextureFromSurface(renderer, phrase_surf);
    // SDL_FreeSurface(phrase_surf);
    // data_ptr->phrase_rect.x = 10;
    // data_ptr->phrase_rect.y = 500;
    // SDL_QueryTexture(data_ptr->phrase, NULL, NULL, &(data_ptr->phrase_rect.w), &(data_ptr->phrase_rect.h));
    // data_ptr->phrase_selected = false;
}
 
void render(SDL_Renderer *renderer, AppData *data_ptr)
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderClear(renderer);
     
    // TODO: draw!

    for (int i = 0; i < data_ptr->entries.size(); i++){
        SDL_RenderCopy(renderer, data_ptr->entries[i].icon_texture_t, NULL, &(data_ptr->entries[i].icon_texture_r));
        SDL_RenderCopy(renderer, data_ptr->entries[i].file_name_t, NULL, &(data_ptr->entries[i].file_name_r));
    }

    // SDL_RenderCopy(renderer, data_ptr->directory, NULL, &(data_ptr->directory_rect));

    // SDL_RenderCopy(renderer, data_ptr->entries[0].file_name_t, NULL, &(data_ptr->entries[0].file_name_r));
    // SDL_RenderCopy(renderer, data_ptr->phrase, NULL, &(data_ptr->phrase_rect));
 
    // show rendered frame
    SDL_RenderPresent(renderer);
}
 
void quit(AppData *data_ptr)
{
    SDL_DestroyTexture(data_ptr->penguin);
    SDL_DestroyTexture(data_ptr->phrase);
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
        //contentInfo.push_back(std::string("ERROR: entry path is not a directory"));
        return directoryEntries;
    }
    std::sort(filesInDir.begin(), filesInDir.end());

    std::string filePath;
    std::string fileName;
    struct stat file;

    entry entry;

    int width = 130;
    int height = 130;

    int startXPosition = 15;
    int startYPosition = 15;

    int row;

    for (int i = 0; i < filesInDir.size(); i++){
        row = i / 5;

        fileName = filesInDir[i];
        filePath = dirPath + "/" + filesInDir[i];
        stat(filePath.c_str(), &file);

        if (S_ISDIR(file.st_mode)){
            entry.type = "Directory";
            entry.path = filePath;
            entry.icon_texture_t = data_ptr->directory;
            entry.icon_texture_r.x = ((i%5) * 160) + startXPosition;
            entry.icon_texture_r.y = (row * 160) + startYPosition;
            entry.icon_texture_r.h = 130;
            entry.icon_texture_r.w = 130;
            SDL_Color color = { 0, 0, 0 };
            SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, fileName.c_str(), color);
            entry.file_name_t = SDL_CreateTextureFromSurface(renderer, phrase_surf);
            SDL_FreeSurface(phrase_surf);
            SDL_QueryTexture(entry.file_name_t, NULL, NULL, &(entry.file_name_r.w), &(entry.file_name_r.h));
            entry.file_name_r.x = ((i%5) * 160) + (75 - (entry.file_name_r.w/2));
            entry.file_name_r.y = ((row) * 160) + 140;

            directoryEntries.push_back(entry);
        } else if (file.st_mode & S_IXUSR){
            entry.type = "Executable";
            entry.path = filePath;
            entry.icon_texture_t = data_ptr->executable;
            entry.icon_texture_r.x = ((i%5) * 160) + startXPosition;
            entry.icon_texture_r.y = (row * 160) + startYPosition;
            entry.icon_texture_r.h = 130;
            entry.icon_texture_r.w = 130;
            SDL_Color color = { 0, 0, 0 };
            SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, fileName.c_str(), color);
            entry.file_name_t = SDL_CreateTextureFromSurface(renderer, phrase_surf);
            SDL_FreeSurface(phrase_surf);
            SDL_QueryTexture(entry.file_name_t, NULL, NULL, &(entry.file_name_r.w), &(entry.file_name_r.h));
            entry.file_name_r.x = ((i%5) * 160) + (75 - (entry.file_name_r.w/2));
            entry.file_name_r.y = ((row) * 160) + 140;

            directoryEntries.push_back(entry);
        } else if (getFileType(fileName) == std::string("Image")){
            entry.type = "Image";
            entry.path = filePath;
            entry.icon_texture_t = data_ptr->image;
            entry.icon_texture_r.x = ((i%5) * 160) + startXPosition;
            entry.icon_texture_r.y = (row * 160) + startYPosition;
            entry.icon_texture_r.h = 130;
            entry.icon_texture_r.w = 130;
            SDL_Color color = { 0, 0, 0 };
            SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, fileName.c_str(), color);
            entry.file_name_t = SDL_CreateTextureFromSurface(renderer, phrase_surf);
            SDL_FreeSurface(phrase_surf);
            SDL_QueryTexture(entry.file_name_t, NULL, NULL, &(entry.file_name_r.w), &(entry.file_name_r.h));
            entry.file_name_r.x = ((i%5) * 160) + (75 - (entry.file_name_r.w/2));
            entry.file_name_r.y = ((row) * 160) + 140;

            directoryEntries.push_back(entry);
        } else if (getFileType(fileName) == std::string("Video")){
            entry.type = getFileType(fileName);
            entry.path = filePath;
            entry.icon_texture_t = data_ptr->video;
            entry.icon_texture_r.x = ((i%5) * 160) + startXPosition;
            entry.icon_texture_r.y = (row * 160) + startYPosition;
            entry.icon_texture_r.h = 130;
            entry.icon_texture_r.w = 130;
            SDL_Color color = { 0, 0, 0 };
            SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, fileName.c_str(), color);
            entry.file_name_t = SDL_CreateTextureFromSurface(renderer, phrase_surf);
            SDL_FreeSurface(phrase_surf);
            SDL_QueryTexture(entry.file_name_t, NULL, NULL, &(entry.file_name_r.w), &(entry.file_name_r.h));
            entry.file_name_r.x = ((i%5) * 160) + (75 - (entry.file_name_r.w/2));
            entry.file_name_r.y = ((row) * 160) + 140;

            directoryEntries.push_back(entry);
        } else if (getFileType(fileName) == std::string("Code file")){
            entry.type = getFileType(fileName);
            entry.path = filePath;
            entry.icon_texture_t = data_ptr->code;
            entry.icon_texture_r.x = ((i%5) * 160) + startXPosition;
            entry.icon_texture_r.y = (row * 160) + startYPosition;
            entry.icon_texture_r.h = 130;
            entry.icon_texture_r.w = 130;
            SDL_Color color = { 0, 0, 0 };
            SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, fileName.c_str(), color);
            entry.file_name_t = SDL_CreateTextureFromSurface(renderer, phrase_surf);
            SDL_FreeSurface(phrase_surf);
            SDL_QueryTexture(entry.file_name_t, NULL, NULL, &(entry.file_name_r.w), &(entry.file_name_r.h));
            entry.file_name_r.x = ((i%5) * 160) + (75 - (entry.file_name_r.w/2));
            entry.file_name_r.y = ((row) * 160) + 140;

            directoryEntries.push_back(entry);
        } else {
            entry.type = getFileType(fileName);
            entry.path = filePath;
            entry.icon_texture_t = data_ptr->other;
            entry.icon_texture_r.x = ((i%5) * 160) + startXPosition;
            entry.icon_texture_r.y = (row * 160) + startYPosition;
            entry.icon_texture_r.h = 130;
            entry.icon_texture_r.w = 130;
            SDL_Color color = { 0, 0, 0 };
            SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, fileName.c_str(), color);
            entry.file_name_t = SDL_CreateTextureFromSurface(renderer, phrase_surf);
            SDL_FreeSurface(phrase_surf);
            SDL_QueryTexture(entry.file_name_t, NULL, NULL, &(entry.file_name_r.w), &(entry.file_name_r.h));
            entry.file_name_r.x = ((i%5) * 160) + (75 - (entry.file_name_r.w/2));
            entry.file_name_r.y = ((row) * 160) + 140;

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
