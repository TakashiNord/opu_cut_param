//////////////////////////////////////////////////////////////////////////////
//
//  opu_cut_param.cpp
//
//  Description:
//      Contains Unigraphics entry points for the application.
//
//////////////////////////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_DEPRECATE 1

/*  Include files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <uf.h>
#include <uf_exit.h>
#include <uf_ui.h>

#include <uf_styler.h>
#include <uf_mb.h> 

#include <uf_attr.h>
#include <uf_obj.h>
#include <uf_param.h>
#include <uf_param_indices.h>

#include <uf_ui_ont.h>

#include <uf_cam.h>
#include <uf_setup.h>
#include <uf_ncgroup.h>
#include <uf_oper.h>

#include <uf_ugopenint.h>

#include <uf_part.h>
#include <uf_assem.h>
#include <uf_ugmgr.h>

/*
#if ! defined ( __hp9000s800 ) && ! defined ( __sgi ) && ! defined ( __sun )
#	include <strstream>
	using std::ostrstream;
	using std::endl;	
	using std::ends;
#else
#	include <strstream.h>
#endif
#include <iostream.h>
*/
#include "opu_cut_param.h"


/* The following definition defines the number of callback entries */
/* in the callback structure:                                      */
/* UF_STYLER_callback_info_t MT_cbs */
#define MT_CB_COUNT ( 10 + 1 ) /* Add 1 for the terminator */
 
/*--------------------------------------------------------------------------
The following structure defines the callback entries used by the       
styler file.  This structure MUST be passed into the user function,    
UF_STYLER_create_dialog along with MT_CB_COUNT.                       
--------------------------------------------------------------------------*/
static UF_STYLER_callback_info_t MT_cbs[MT_CB_COUNT] = 
{
 {UF_STYLER_DIALOG_INDEX, UF_STYLER_CONSTRUCTOR_CB  , 0, MT_construct_cb},
 {UF_STYLER_DIALOG_INDEX, UF_STYLER_APPLY_CB        , 0, MT_apply_cb},
 {MT_STOCK_TOG          , UF_STYLER_VALUE_CHANGED_CB, 0, MT_stock_tog_cb},
 {MT_STOCK_REAL_1       , UF_STYLER_ACTIVATE_CB     , 0, MT_real_cb},
 {MT_STOCK_REAL_2       , UF_STYLER_ACTIVATE_CB     , 0, MT_real_cb},
 {MT_STOCK_REAL_3       , UF_STYLER_ACTIVATE_CB     , 0, MT_real_cb},
 {MT_STOCK_REAL_4       , UF_STYLER_ACTIVATE_CB     , 0, MT_real_cb},
 {MT_STOCK_REAL_5       , UF_STYLER_ACTIVATE_CB     , 0, MT_real_cb},
 {MT_STOCK_REAL_6       , UF_STYLER_ACTIVATE_CB     , 0, MT_real_cb},
 {MT_STOCK_REAL_7       , UF_STYLER_ACTIVATE_CB     , 0, MT_real_cb},
 {UF_STYLER_NULL_OBJECT, UF_STYLER_NO_CB, 0, 0 }
};



/*--------------------------------------------------------------------------
UF_MB_styler_actions_t contains 4 fields.  These are defined as follows:
 
Field 1 : the name of your dialog that you wish to display.
Field 2 : any client data you wish to pass to your callbacks.
Field 3 : your callback structure.
Field 4 : flag to inform menubar of your dialog location.  This flag MUST  
          match the resource set in your dialog!  Do NOT ASSUME that changing 
          this field will update the location of your dialog.  Please use the 
          UIStyler to indicate the position of your dialog.
--------------------------------------------------------------------------*/
static UF_MB_styler_actions_t actions[] = {
    { "opu_cut_param.dlg",  NULL,   MT_cbs,  UF_MB_STYLER_IS_NOT_TOP },
    { NULL,  NULL,  NULL,  0 } /* This is a NULL terminated list */
};






static void PrintErrorMessage( int errorCode );

/****************************************************************************/

#define UF_CALL(X) (report( __FILE__, __LINE__, #X, (X)))

static int report( char *file, int line, char *call, int irc)
{
  if (irc)
  {
     char    messg[133];
     printf("%s, line %d:  %s\n", file, line, call);
     (UF_get_fail_message(irc, messg)) ?
       printf("    returned a %d\n", irc) :
       printf("    returned error %d:  %s\n", irc, messg);
  }
  return(irc);
}


#define COUNT_PRG 500

struct PRG
{
   int     num;
   tag_t   tag;
   char    name[UF_OPER_MAX_NAME_LEN+1];
} ;

static struct PRG prg_list[COUNT_PRG] ;
int prg_list_count=0;

static logical cycleGeneratePrg( tag_t   tag, void   *data );

/* */
/*----------------------------------------------------------------------------*/
static logical cycleGeneratePrg( tag_t   tag, void   *data )
{
   char     name[UF_OPER_MAX_NAME_LEN + 1];
   int      ecode;

   name[0]='\0';
   ecode = UF_OBJ_ask_name(tag, name);// спросим имя обьекта
   //UF_UI_write_listing_window("\n");  UF_UI_write_listing_window(name);

   if (prg_list_count>=COUNT_PRG) {
     uc1601("Число Операций-превышает допустимое (>500)\n Уменьшите количество выбора",1);
     return( FALSE );
   }
   prg_list[prg_list_count].num=prg_list_count;
   prg_list[prg_list_count].tag=tag;
   prg_list[prg_list_count].name[0]='\0';
   sprintf(prg_list[prg_list_count].name,"%s",name);
   prg_list_count++;

   return( TRUE );
}

double dp[8];

int ufusr_main( );
int _SET ( tag_t  prg ) ;
int _READ ( int dialog_id ) ;

int _construct_cb ( int dialog_id ) ;
int _apply_cb ( int dialog_id ) ;
int _type_cb ( int dialog_id ) ;

//----------------------------------------------------------------------------
//  Activation Methods
//----------------------------------------------------------------------------

//  Explicit Activation
//      This entry point is used to activate the application explicitly, as in
//      "File->Execute UG/Open->User Function..."
extern "C" DllExport void ufusr( char *parm, int *returnCode, int rlen )
{
    /* Initialize the API environment */
    int errorCode = UF_initialize();

    if ( 0 == errorCode )
    {
        /* TODO: Add your application code here */
        ufusr_main( );

        /* Terminate the API environment */
        errorCode = UF_terminate();
    }

    /* Print out any error messages */
    PrintErrorMessage( errorCode );
    *returnCode = 0 ;
}

//----------------------------------------------------------------------------
//  Utilities
//----------------------------------------------------------------------------

// Unload Handler
//     This function specifies when to unload your application from Unigraphics.
//     If your application registers a callback (from a MenuScript item or a
//     User Defined Object for example), this function MUST return
//     "UF_UNLOAD_UG_TERMINATE".
extern "C" int ufusr_ask_unload( void )
{
     /* unload immediately after application exits*/
    return ( UF_UNLOAD_IMMEDIATELY );

     /*via the unload selection dialog... */
     //return ( UF_UNLOAD_SEL_DIALOG );
     /*when UG terminates...              */
     //return ( UF_UNLOAD_UG_TERMINATE );
}


/*--------------------------------------------------------------------------
You have the option of coding the cleanup routine to perform any housekeeping
chores that may need to be performed.  If you code the cleanup routine, it is
automatically called by Unigraphics.
--------------------------------------------------------------------------*/
extern void ufusr_cleanup (void)
{
    return;
}


/* PrintErrorMessage
**
**     Prints error messages to standard error and the Unigraphics status
**     line. */
static void PrintErrorMessage( int errorCode )
{
    if ( 0 != errorCode )
    {
        /* Retrieve the associated error message */
        char message[133];
        UF_get_fail_message( errorCode, message );

        /* Print out the message */
        UF_UI_set_status( message );

        fprintf( stderr, "%s\n", message );
    }
}



/*********************************************



*********************************************/
int ufusr_main( )
{
  int errorCode;
  int response ;

  /****************************************************************************/

    if ( ( errorCode = UF_STYLER_create_dialog (
            actions->styler_file,
           MT_cbs,      /* Callbacks from dialog */
           MT_CB_COUNT, /* number of callbacks*/
            NULL,        /* This is your client data */
            &response ) ) != 0 )
    {
       uc1601("Ошибка загрузки шаблона диалога\nОшибка памяти или нет dlg-файла.",1);
    }

  /****************************************************************************/

  return(0);
}



int _READ ( int dialog_id )
{
  UF_STYLER_item_value_type_t data  ;
  int i , j;
  int ret = 0 ;

  char *ID[]={
    MT_STOCK_TOG,         "1",
    MT_STOCK_REAL_1,      "0",
    MT_STOCK_REAL_2,      "0",
    MT_STOCK_REAL_3,      "0",
    MT_STOCK_REAL_4,      "0",
    MT_STOCK_REAL_5,      "0",
    MT_STOCK_REAL_6,      "0",
    MT_STOCK_REAL_7,      "0",
    NULL
  } ;

  data.item_attr=UF_STYLER_VALUE;

  for (i=0,j=0;i<8;i++,j+=2)
  {
    data.item_id=ID[j];
    UF_STYLER_ask_value(dialog_id,&data);
    if (0==strcmp(ID[j+1],"1"))
      dp[i]=data.value.integer ;
    else
    {
      dp[i]=data.value.real ;
      if ((i!=0) && (dp[i]<0)) ret=-1;
    }
  }

  UF_STYLER_free_value (&data) ;

  return (ret);
}


int _SET ( tag_t  prg )
{
  int status , topoff ;
  int cnt = 0 ;
  int ecode = 0 ;
  double  dV = 0;
  int     iV = 0;

 UF_PARAM_cutlev_method_t        cutlev_method ;
 UF_PARAM_cutlev_topoff_status_t topoff_status ;

 /******************************************************************************/

  status=(int)dp[0] ;

  ecode=UF_CALL( UF_PARAM_ask_int_value(prg,UF_PARAM_CUTLEV_METHOD,&iV) );

  if (ecode==0) {
    UF_CALL( UF_PARAM_set_int_value(prg,UF_PARAM_CUTLEV_METHOD,cutlev_method) );
    cnt++;
   }

  ecode=UF_CALL( UF_PARAM_ask_double_value(prg,UF_PARAM_CUTLEV_MIN_DEPTH,&dV) );
  if (ecode==0) {
    UF_CALL( UF_PARAM_set_double_value(prg,UF_PARAM_CUTLEV_MIN_DEPTH,dp[2]) );
    cnt++;
  }
 /******************************************************************************/

 return (cnt);
}


int _apply_cb ( int dialog_id )
{
  /*  Variable Declarations */
  char   str[133];

  tag_t       prg = NULL_TAG;
  int   i , j , count = 0 ;
  int   obj_count = 0;
  tag_t *tags = NULL ;
  int generat; logical generated ;
  int type , subtype ;

  char *mes2[]={
      "Производить генерацию операции после изменения?",
      NULL
  };
  UF_UI_message_buttons_t buttons2 = { TRUE, FALSE, TRUE, "Генерировать..", NULL, "Нет", 1, 0, 2 };

  int module_id;
  UF_ask_application_module(&module_id);
  if (UF_APP_CAM!=module_id) {
     // UF_APP_GATEWAY UF_APP_CAM UF_APP_MODELING UF_APP_NONE
     uc1601("Запуск DLL - производится из модуля обработки\n - 2005г.",1);
     return (-1);
  }

  /* Ask displayed part tag */
  if (NULL_TAG==UF_PART_ask_display_part()) {
    uc1601("Cam-часть не активна.....\n ",1);
    return (0);
  }

  /********************************************************************************/

  // выбранные обьекты и их кол-во
  UF_CALL( UF_UI_ONT_ask_selected_nodes(&obj_count, &tags) );
  if (obj_count<=0) {
    uc1601("Не выбрано операций!\n",1);
    return(0);
  }

  /********************************************************************************/

  i=_READ ( dialog_id ) ;
  if (i!=0) {
    uc1601("Не правильное задание значения параметра\nДля всех параметров,кроме SIDE_STOCK_INCR, значение должно быть >=0",1);
    return(0);
  }

  /********************************************************************************/

  generat=1;
  UF_UI_message_dialog("Cam.....", UF_UI_MESSAGE_QUESTION, mes2, 1, TRUE, &buttons2, &generat);
  if (generat==2) { generat=0; }

  /********************************************************************************/

 UF_UI_toggle_stoplight(1);

 for(i=0,count=0;i<obj_count;i++)
 {
   prg = tags[i]; // идентификатор объекта

   prg_list_count=0;// заполняем структуру
   UF_CALL( UF_NCGROUP_cycle_members( prg, cycleGeneratePrg,NULL ) );

   if (prg_list_count==0) {

      /* type =               subtype =
      //     программа=121              160
      //     операция =100              110 */
      UF_CALL( UF_OBJ_ask_type_and_subtype (prg, &type, &subtype ) );

      if (type!=100) continue ;

      count+=_SET( prg );
      if (generat==1) { UF_CALL( UF_PARAM_generate (prg,&generated ) ); }
   } else for (j=0;j<prg_list_count;j++) {
             count+=_SET( prg_list[j].tag );
             if (generat==1) { UF_CALL( UF_PARAM_generate (prg_list[j].tag,&generated ) ); }
           }

 }

 UF_free(tags);

 //UF_DISP_refresh ();
 UF_UI_ONT_refresh();

 UF_UI_toggle_stoplight(0);

 str[0]='\0'; sprintf(str,"Изменено значений =%d \n Просмотрено операций в цикле =%d \n программа завершена.",count,obj_count);
 uc1601(str,1);

 return (0);
}


int _type_cb ( int dialog_id )
{
  UF_STYLER_item_value_type_t data  ;
  int  sens;

  data.item_attr=UF_STYLER_VALUE;
  data.item_id=STR[1];
  UF_STYLER_ask_value(dialog_id,&data);
  sens=data.value.integer;
  if (sens<0) sens=0;

  data.item_attr=UF_STYLER_SENSITIVITY;

  data.item_id=STR[i];
  data.value.integer=arr[i];
  UF_STYLER_set_value(dialog_id,&data);

  UF_STYLER_free_value (&data) ;

  return (0);
}




/*-------------------------------------------------------------------------*/
/*---------------------- UIStyler Callback Functions ----------------------*/
/*-------------------------------------------------------------------------*/

/* -------------------------------------------------------------------------
 * Callback Name: MT_construct_cb
 * This is a callback function associated with an action taken from a
 * UIStyler object. 
 *
 * Input: dialog_id   -   The dialog id indicate which dialog this callback
 *                        is associated with.  The dialog id is a dynamic,
 *                        unique id and should not be stored.  It is
 *                        strictly for the use in the NX Open API:
 *                               UF_STYLER_ask_value(s) 
 *                               UF_STYLER_set_value   
 *        client_data -   Client data is user defined data associated
 *                        with your dialog.  Client data may be bound
 *                        to your dialog with UF_MB_add_styler_actions
 *                        or UF_STYLER_create_dialog.                 
 *        callback_data - This structure pointer contains information
 *                        specific to the UIStyler Object type that  
 *                        invoked this callback and the callback type.
 * -----------------------------------------------------------------------*/
int MT_construct_cb ( int dialog_id,
             void * client_data,
             UF_STYLER_item_value_type_p_t callback_data)
{
     /* Make sure User Function is available. */  
     if ( UF_initialize() != 0) 
          return ( UF_UI_CB_CONTINUE_DIALOG );

     /* ---- Enter your callback code here ----- */

     UF_terminate ();

    /* Callback acknowledged, do not terminate dialog */
    return (UF_UI_CB_CONTINUE_DIALOG); 
    /* A return value of UF_UI_CB_EXIT_DIALOG will not be accepted    */
    /* for this callback type.  You must continue dialog construction.*/

}


/* -------------------------------------------------------------------------
 * Callback Name: MT_apply_cb
 * This is a callback function associated with an action taken from a
 * UIStyler object. 
 *
 * Input: dialog_id   -   The dialog id indicate which dialog this callback
 *                        is associated with.  The dialog id is a dynamic,
 *                        unique id and should not be stored.  It is
 *                        strictly for the use in the NX Open API:
 *                               UF_STYLER_ask_value(s) 
 *                               UF_STYLER_set_value   
 *        client_data -   Client data is user defined data associated
 *                        with your dialog.  Client data may be bound
 *                        to your dialog with UF_MB_add_styler_actions
 *                        or UF_STYLER_create_dialog.                 
 *        callback_data - This structure pointer contains information
 *                        specific to the UIStyler Object type that  
 *                        invoked this callback and the callback type.
 * -----------------------------------------------------------------------*/
int MT_apply_cb ( int dialog_id,
             void * client_data,
             UF_STYLER_item_value_type_p_t callback_data)
{
     /* Make sure User Function is available. */  
     if ( UF_initialize() != 0) 
          return ( UF_UI_CB_CONTINUE_DIALOG );

     /* ---- Enter your callback code here ----- */

     UF_terminate ();

    /* Callback acknowledged, do not terminate dialog                 */
    /* A return value of UF_UI_CB_EXIT_DIALOG will not be accepted    */
    /* for this callback type.  You must respond to your apply button.*/
    return (UF_UI_CB_CONTINUE_DIALOG); 

}


/* -------------------------------------------------------------------------
 * Callback Name: MT_stock_tog_cb
 * This is a callback function associated with an action taken from a
 * UIStyler object. 
 *
 * Input: dialog_id   -   The dialog id indicate which dialog this callback
 *                        is associated with.  The dialog id is a dynamic,
 *                        unique id and should not be stored.  It is
 *                        strictly for the use in the NX Open API:
 *                               UF_STYLER_ask_value(s) 
 *                               UF_STYLER_set_value   
 *        client_data -   Client data is user defined data associated
 *                        with your dialog.  Client data may be bound
 *                        to your dialog with UF_MB_add_styler_actions
 *                        or UF_STYLER_create_dialog.                 
 *        callback_data - This structure pointer contains information
 *                        specific to the UIStyler Object type that  
 *                        invoked this callback and the callback type.
 * -----------------------------------------------------------------------*/
int MT_stock_tog_cb ( int dialog_id,
             void * client_data,
             UF_STYLER_item_value_type_p_t callback_data)
{
     /* Make sure User Function is available. */  
     if ( UF_initialize() != 0) 
          return ( UF_UI_CB_CONTINUE_DIALOG );

     /* ---- Enter your callback code here ----- */

     UF_terminate ();

    /* Callback acknowledged, do not terminate dialog */
    return (UF_UI_CB_CONTINUE_DIALOG); 
    
    /* or Callback acknowledged, terminate dialog.  */
    /* return ( UF_UI_CB_EXIT_DIALOG );             */

}


/* -------------------------------------------------------------------------
 * Callback Name: MT_real_cb
 * This is a callback function associated with an action taken from a
 * UIStyler object. 
 *
 * Input: dialog_id   -   The dialog id indicate which dialog this callback
 *                        is associated with.  The dialog id is a dynamic,
 *                        unique id and should not be stored.  It is
 *                        strictly for the use in the NX Open API:
 *                               UF_STYLER_ask_value(s) 
 *                               UF_STYLER_set_value   
 *        client_data -   Client data is user defined data associated
 *                        with your dialog.  Client data may be bound
 *                        to your dialog with UF_MB_add_styler_actions
 *                        or UF_STYLER_create_dialog.                 
 *        callback_data - This structure pointer contains information
 *                        specific to the UIStyler Object type that  
 *                        invoked this callback and the callback type.
 * -----------------------------------------------------------------------*/
int MT_real_cb ( int dialog_id,
             void * client_data,
             UF_STYLER_item_value_type_p_t callback_data)
{
     /* Make sure User Function is available. */  
     if ( UF_initialize() != 0) 
          return ( UF_UI_CB_CONTINUE_DIALOG );

     /* ---- Enter your callback code here ----- */

     UF_terminate ();

    /* Callback acknowledged, do not terminate dialog */
    return (UF_UI_CB_CONTINUE_DIALOG); 
    
    /* or Callback acknowledged, terminate dialog.    */
    /* return ( UF_UI_CB_EXIT_DIALOG );               */

}


