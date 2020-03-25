#include <iostream>
#include <cstring>

#include "rmTag.h"

void rmTag(char *m_pBuffer, char *tag)
{
  /* replace newline with  space */
  for (int i=0; i<strlen(m_pBuffer); i++)
    if(m_pBuffer[i]=='\n')
      m_pBuffer[i] = ' ';
  
  char front_tag[20], back_tag[20];
  sprintf(front_tag, "<%s", tag);   // create <tag for strstr
  sprintf(back_tag, "</%s>", tag);  // create </tag> for strstr
  char *front_ptr = m_pBuffer, *back_ptr=NULL;
  int time=0;
  while((front_ptr = strstr(front_ptr, front_tag))!=NULL)
  {
    //std::cout << time++ << std::endl;
    char *tmp = front_ptr;
    front_ptr++;
    *tmp = '\0';
    if ((back_ptr = strstr(front_ptr, back_tag))==NULL) {
      //error
      std::cout << "error" << std::endl;
      break;
    }
    //std::cout << time++ << std::endl;
    strcat(tmp, back_ptr+strlen(back_tag));
    front_ptr--;
  }
  // std::cout << m_pBuffer << std::endl;
}