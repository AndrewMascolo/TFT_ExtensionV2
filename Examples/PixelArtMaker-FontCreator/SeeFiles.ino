#if 1
void SetupButtonsOtherButtons(char *buf)
{
  myGLCD.fillScr(BLACK);
  File root = SD.open("/");

  OkButton.Coords(550, 60, 660, 100);
  OkButton.Colors(GREEN, RED, ROUNDED, FILL);
  OkButton.Text("OK", BLACK, Big);

  CancelButton.Coords(680, 60, 790, 100);
  CancelButton.Colors(GREEN, RED, ROUNDED, FILL);
  CancelButton.Text("CANCEL", BLACK, Big);

  NewButton.Coords(550, 10, 660, 50);
  NewButton.Colors(GREEN, RED, ROUNDED, FILL);
  NewButton.Text("NEW", BLACK, Big);

  DeleteButton.Coords(680, 10, 790, 50);
  DeleteButton.Colors(GREEN, RED, ROUNDED, FILL);
  DeleteButton.Text("DELETE", BLACK, Big);

  ListSelect.Coords(0, 0, 300, 40);
  ListSelect.Colors(GREEN, RED, ROUNDED, FILL);
  ListSelect.Text("", BLACK, Big);

  BackButton.Coords(680, 110, 790, 150);
  BackButton.Colors(GREEN, RED, ROUNDED, FILL);
  BackButton.Text("BACK", BLACK, Big);

  OkButton.ReDraw();
  CancelButton.ReDraw();
  ListSelect.ReDraw();
  //BackButton.ReDraw();
  NewButton.ReDraw();
  DeleteButton.ReDraw();

  printDirectory(root, message);
  strcpy(buf, message);

  FileButton.Text(message, WHITE, Big);
  myGLCD.fillScr(BLACK);
}

void printDirectory(File dir, char * buf)
{
  myGLCD.setColor(WHITE);
  int i = 0, LastI, selection = -1;
  char myFiles[15][20] = {'\0'};
  bool backFlag = false;

  dir.seek(0);
  while (true)
  {
    //myGLCD.printNumI(i, 300, 0);
    File entry =  dir.openNextFile();
    if (entry)
    {
      if (!entry.isDirectory())
      {
        char* Name = entry.name();
        for(byte i = 0, j = strlen(Name); i < j; i++)
        {
          Name[i] = tolower(Name[i]);  
        }
        Serial.println(Name);
        strncpy(myFiles[i], Name, strlen(Name));
        ListSelect.Text(myFiles[i], Big, WHITE);
        ListSelect.Coords(0, i * 50, 300, 40 + (i * 50));
        ListSelect.ReDraw();
        i++;
        entry.close();
      }
    }
    else
      break;
  }

  //---------------------------------------------------------
  LastI = i;
  i = 0;

  while (true)
  {
    if (i < LastI)
    {
      ListSelect.Coords(0, i * 50, 300, 40 + (i * 50));

      if (ListSelect.Touch())
      {
        myGLCD.setColor(WHITE);
        myGLCD.setBackColor(BLACK);
        myGLCD.print(F("            "), 350, 5);
        myGLCD.print(myFiles[i], 350, 5);
        ListSelect.Text(myFiles[i], Big, WHITE);
        selection = i;

        for (int j = 0; j < LastI; j++)
        {
          ListSelect.Coords(0, j * 50, 300, 40 + (j * 50));
          ListSelect.Text(myFiles[j], Big, WHITE);
          ListSelect.SetState(j == selection);
          ListSelect.ReDraw();
        }
      }

      i++;

      if ( OkButton.DoubleClick() ) // load it
      {
        strcpy(message, ((selection >= 0) ? myFiles[selection] : NULL));
        return;
      }

      if (CancelButton.DoubleClick()) // cancel
      {
        myGLCD.setColor(WHITE);
        myGLCD.setBackColor(BLACK);
        myGLCD.print(F("            "), 410, 5);
      }

      if (DeleteButton.Delay()) // Erase it
      {
        SD.remove(myFiles[selection]);
        break;
      }

      if (NewButton.DoubleClick())
      {
        myGLCD.fillScr(BLACK);
        myKB.Colors(WHITE, BLACK, BLUE, FILL);
        myKB.SetupMobileKB(0, 150, 800, 300);
        myTouch.setPrecision(PREC_MEDIUM);
        CancelButton.ReDraw();

        char tmp[30] = {'\0'};
        while (1)
        {
          if (myKB.Mobile_KeyBoard(tmp, BLUE))
          {
            strncpy(message, tmp, 7);
            strcat(message, ".c");
            return;
          }

          if (BackButton.Touch())
          {
            backFlag = true;
            break;
          }

          if (CancelButton.DoubleClick()) // cancel
            SetupButtonsOtherButtons(buf);
        }
        OkButton.Unlock();
        CancelButton.Unlock();
        BackButton.Unlock();
        NewButton.Unlock();
        DeleteButton.Unlock();

        if (backFlag == true)
        {
          NewButton.ReDraw();
          DeleteButton.ReDraw();
          OkButton.ReDraw();
          CancelButton.ReDraw();
          backFlag = false;
          return;
        }
      }
    }
    else i = 0;
  }
}
#endif
