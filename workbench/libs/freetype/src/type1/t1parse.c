#include <ftdebug.h>
#include <t1types.h>
#include <t1parse.h>

#include <stdio.h>  /* for sscanf */

/*************************************************************************/
/*                                                                       */
/* <Function> T1_New_Table                                               */
/*                                                                       */
/* <Description>                                                         */
/*    Initialise a T1_Table.                                             */
/*                                                                       */
/* <Input>                                                               */
/*    table  :: address of target table                                  */
/*    count  :: table size = maximum number of elements                  */
/*    memory :: memory object to use for all subsequent reallocations    */
/*                                                                       */
/* <Return>                                                              */
/*    Error code. 0 means success                                        */
/*                                                                       */

  LOCAL_FUNC
  T1_Error  T1_New_Table( T1_Table*  table,
                          T1_Int     count,
                          FT_Memory  memory )
  {
	 T1_Error  error;

	 table->memory = memory;
	 if ( ALLOC_ARRAY( table->elements, count, T1_Byte* ) )
		 return error;

	 if ( ALLOC_ARRAY( table->lengths, count, T1_Byte* ) )
	 {
	   FREE( table->elements );
	   return error;
	}

	table->max_elems = count;
	table->num_elems = 0;

	table->block        = 0;
	table->capacity     = 0;
	table->cursor       = 0;

	return error;
  }



/*************************************************************************/
/*                                                                       */
/* <Function> T1_Add_Table                                               */
/*                                                                       */
/* <Description>                                                         */
/*    Adds an object to a T1_Table, possibly growing its memory block    */
/*                                                                       */
/* <Input>                                                               */
/*    table  :: target table                                             */
/*    index  :: index of object in table                                 */
/*    object :: address of object to copy in memory                      */
/*    length :: length in bytes of source object                         */
/*                                                                       */
/* <Return>                                                              */
/*    Error code. 0 means success. An error is returned when a           */
/*    realloc failed..                                                   */
/*                                                                       */

      static
      T1_Error  reallocate_t1_table( T1_Table*  table,
                                     T1_Int     new_size )
      {
        FT_Memory  memory   = table->memory;
        T1_Byte*   old_base = table->block;
        T1_Error   error;

        /* realloc the base block */
        if ( REALLOC( table->block, table->capacity, new_size ) )
          return error;
        table->capacity = new_size;

        /* shift all offsets when needed */
        if (old_base)
        {
          T1_Long   delta  = table->block - old_base;
          T1_Byte** offset = table->elements;
          T1_Byte** limit  = offset + table->max_elems;

          if (delta)
            for ( ; offset < limit; offset ++ )
              if (offset[0])
                offset[0] += delta;
        }

        return T1_Err_Ok;
      }


  LOCAL_FUNC
  T1_Error  T1_Add_Table( T1_Table*  table,
                          T1_Int     index,
                          void*      object,
                          T1_Int     length )
  {
	if (index < 0 || index > table->max_elems)
    {
	  FT_ERROR(( "T1.Add_Table: invalid index\n" ));
	  return T1_Err_Syntax_Error;
    }

    /* grow the base block if needed */
    if ( table->cursor + length > table->capacity )
    {
      T1_Error  error;
      T1_Int    new_size = table->capacity;

      while ( new_size < table->cursor+length )
        new_size += 1024;

      error = reallocate_t1_table( table, new_size );
      if (error) return error;
    }

    /* add the object to the base block and adjust offset */
    table->elements[ index ] = table->block + table->cursor;
    table->lengths [ index ] = length;
    MEM_Copy( table->block + table->cursor, object, length );

    table->cursor += length;
    return T1_Err_Ok;
  }


/*************************************************************************/
/*                                                                       */
/* <Function> T1_Done_Table                                              */
/*                                                                       */
/* <Description>                                                         */
/*    Finalise a T1_Table. (realloc it to its current cursor).           */
/*                                                                       */
/* <Input>                                                               */
/*    table :: target table                                              */
/*                                                                       */
/* <Note>                                                                */
/*    This function does NOT release the heap's memory block. It is up   */
/*    to the caller to clean it, or reference it in its own structures.  */
/*                                                                       */
  LOCAL_FUNC
  void  T1_Done_Table( T1_Table*  table )
  {
    FT_Memory  memory = table->memory;
    T1_Error   error;
    T1_Byte*   old_base;

    /* should never fail, as rec.cursor <= rec.size */
    old_base = table->block;
    if (!old_base)
      return;
    
    (void)REALLOC( table->block, table->capacity, table->cursor );
    table->capacity = table->cursor;
    
    if (old_base != table->block)
    {
      T1_Long   delta   = table->block - old_base;
      T1_Byte** element = table->elements;
      T1_Byte** limit   = element + table->max_elems;
      
      for ( ; element < limit; element++ )
        if (element[0])
          element[0] += delta;
    }
  }


  LOCAL_FUNC
  T1_String*   CopyString( T1_Parser*  parser )
  {
    T1_String*  string = NULL;
    T1_Token*   token  = parser->args++;
    FT_Memory   memory = parser->tokenizer->memory;
    T1_Error    error;

    if ( token->kind == tok_string )
    {
      int  len = token->len-2;

      if ( ALLOC( string, len+1 ) )
      {
        parser->error = error;
        return 0;
      }

      MEM_Copy( string, parser->tokenizer->base + token->start+1, len );
      string[len] = '\0';

      parser->error = T1_Err_Ok;
    }
    else
    {
      FT_ERROR(( "T1.CopyString : syntax error, string token expected !\n" ));
      parser->error = T1_Err_Syntax_Error;
    }
    return string;
  }



  static
  T1_Error  parse_int( T1_Byte*  base,
                       T1_Byte*  limit,
                       T1_Long*  result )
  {
    T1_Bool  sign = 0;
    T1_Long  sum  = 0;

    if (base >= limit)
      goto Fail;

    /* check sign */
    if ( *base == '+' )
      base++;

    else if ( *base == '-' )
    {
      sign++;
      base++;
    }

    /* parse digits */
    if ( base >= limit )
      goto Fail;

    do
    {
      sum = ( 10*sum + (*base++ - '0') );

    } while (base < limit);

    if (sign)
      sum = -sum;

    *result = sum;
    return T1_Err_Ok;

  Fail:
    FT_ERROR(( "T1.parse_integer : integer expected\n" ));
    *result = 0;
    return T1_Err_Syntax_Error;
  }




  static
  T1_Error  parse_float( T1_Byte*  base,
                         T1_Byte*  limit,
                         T1_Int    scale,
                         T1_Long*  result )
  {
#if 1
    /* XXX : We're simply much too lazy to code this function  */
    /*       properly for now.. We'll do that when the rest of */
    /*       the driver works properly..                       */
    char    temp[32];
    int     len = limit-base;
    double  value;

    if (len > 31) goto Fail;

    strncpy( temp, (char*)base, len );
    temp[len] = '\0';
    if ( sscanf( temp, "%lf", &value ) != 1 )
      goto Fail;

    *result = (T1_Long)(scale*value);
    return 0;

#else
  T1_Byte*  cur;
  T1_Bool   sign        = 0;  /* sign                        */
  T1_Long   number_int  = 0;  /* integer part                */
  T1_Long   number_frac = 0;  /* fractional part             */
  T1_Long   exponent    = 0;  /* exponent value              */
  T1_Int    num_frac    = 0;  /* number of fractional digits */

  /* check sign */
  if (*base == '+')
    base++;

  else if (*base == '-')
  {
    sign++;
    base++;
  }

  /* find integer part */
  cur = base;
  while ( cur < limit )
  {
    T1_Byte  c = *cur;
    if ( c == '.' || c == 'e' || c == 'E' )
      break;

    cur++;
  }

  if ( cur > base )
  {
    error = parse_integer( base, cur, &number_int );
    if (error) goto Fail;
  }

  /* read fractional part, if any */
  if ( *cur == '.' )
  {
    cur++;
    base = cur;
    while ( cur < limit )
    {
      T1_Byte  c = *cur;
      if ( c == 'e' || c == 'E' )
        break;
      cur++;
    }

    num_frac = cur - base;

    if ( cur > base )
    {
      error = parse_integer( base, cur, &number_frac );
      if (error) goto Fail;
      base = cur;
    }
  }

  /* read exponent, if any */
  if ( *cur == 'e' || *cur == 'E' )
  {
    cur++;
    base = cur;
    error = parse_integer( base, limit, &exponent );
    if (error) goto Fail;

    /* now check that exponent is within 'correct bounds' */
    /* i.e. between -6 and 6                              */
    if ( exponent < -6 || exponent > 6 )
      goto Fail;
  }

  /* now adjust integer value and exponent for fractional part */
  while ( num_frac > 0 )
  {
    number_int *= 10;
    exponent   --;
    num_frac--;
  }

  number_int += num_frac;

  /* skip point if any, read fractional part */
  if ( cur+1 < limit )
  {
    if (*cur
  }

  /* now compute scaled float value */
  /* XXXXX : incomplete !!!         */
#endif

  Fail:
    FT_ERROR(( "T1.parse_float : syntax error !\n" ));
    return T1_Err_Syntax_Error;
  }



  static
  T1_Error  parse_integer( T1_Byte*  base,
                           T1_Byte*  limit,
                           T1_Long*  result )
  {
    T1_Byte*  cur;

    /* the lexical analyser accepts floats as well as integers */
    /* now, check that we really have an int in this token     */
    cur = base;
    while ( cur < limit )
    {
      T1_Byte  c = *cur++;

      if ( c == '.' || c == 'e' || c == 'E' )
        goto Float_Number;
    }

    /* now read the number's value */
    return parse_int( base, limit, result );

  Float_Number:
    /* We really have a float there, simply call parse_float in this */
    /* case with a scale of '10' to perform round..                */
    {
      T1_Error error;

      error = parse_float( base, limit, 10, result );
      if (!error)
      {
        if (*result >= 0) *result = (*result+5)/10;      /* round value */
                    else  *result = -((5-*result)/10);
      }
      return error;
    }
  }


  LOCAL_FUNC
  T1_Long  CopyInteger( T1_Parser*  parser )
  {
    T1_Long   sum   = 0;
    T1_Token* token = parser->args++;

    if ( token->kind == tok_number )
    {
      T1_Byte*  base  = parser->tokenizer->base + token->start;
      T1_Byte*  limit = base + token->len;

      /* now read the number's value */
      parser->error = parse_integer( base, limit, &sum );
      return sum;
    }

    FT_ERROR(( "T1.CopyInteger : number expected\n" ));
    parser->args--;
    parser->error = T1_Err_Syntax_Error;
    return 0;
  }



  LOCAL_FUNC
  T1_Bool   CopyBoolean( T1_Parser*  parser )
  {
    T1_Error  error  = T1_Err_Ok;
    T1_Bool   result = 0;
    T1_Token* token  = parser->args++;

    if ( token->kind == tok_keyword )
    {
      if ( token->kind2 == key_false )
        result = 0;

      else if ( token->kind2 == key_true )
        result = !0;

      else
        goto Fail;
    }
    else
    {
      Fail:
        FT_ERROR(( "T1.CopyBoolean : syntax error, 'false' or 'true' expected\n" ));
        error = T1_Err_Syntax_Error;
    }
    parser->error = error;
    return result;
  }




  LOCAL_FUNC
  T1_Long   CopyFloat( T1_Parser*  parser,
                       T1_Int      scale )
  {
    T1_Error  error;
    T1_Long   sum = 0;
    T1_Token* token = parser->args++;

    if ( token->kind == tok_number )
    {
      T1_Byte*  base  = parser->tokenizer->base + token->start;
      T1_Byte*  limit = base + token->len;

      error = parser->error = parse_float( base, limit, scale, &sum );
      if (error) goto Fail;

      return sum;
    }

  Fail:
    FT_ERROR(( "T1.CopyFloat : syntax error !\n" ));
    parser->error = T1_Err_Syntax_Error;
    return 0;
  }






  LOCAL_FUNC
  void  CopyBBox( T1_Parser*  parser,
                  T1_BBox*    bbox )
  {
    T1_Token* token = parser->args++;
    T1_Int    n;
    T1_Error  error;

    if ( token->kind == tok_program ||
         token->kind == tok_array   )
    {
      /* get rid of '['/']', or '{'/'}' */
      T1_Byte*  base  = parser->tokenizer->base + token->start + 1;
      T1_Byte*  limit = base + token->len - 1;
      T1_Byte*  cur;
      T1_Byte*  start;

      /* read each parameter independently */
      cur = base;
      for ( n = 0; n < 4; n++ )
      {
        T1_Long*  result;

        /* skip whitespace */
        while (cur < limit && *cur == ' ') cur++;

        /* skip numbers */
        start = cur;
        while (cur < limit && *cur != ' ') cur++;

        /* compute result address */
        switch (n)
        {
          case 0 : result = &bbox->xMin; break;
          case 1 : result = &bbox->yMin; break;
          case 2 : result = &bbox->xMax; break;
          default: result = &bbox->yMax;
        }

        error = parse_integer( start, cur, result );
        if (error) goto Fail;
      }
      parser->error = 0;
      return;
    }

  Fail:
    FT_ERROR(( "T1.CopyBBox : syntax error !\n" ));
    parser->error = T1_Err_Syntax_Error;
  }




  LOCAL_FUNC
  void  CopyMatrix( T1_Parser*  parser,
                    T1_Matrix*  matrix )
  {
    T1_Token* token = parser->args++;
    T1_Error  error;

    if ( token->kind == tok_array )
    {
      /* get rid of '[' and ']' */
      T1_Byte*  base  = parser->tokenizer->base + token->start + 1;
      T1_Byte*  limit = base + token->len - 1;
      T1_Byte*  cur;
      T1_Byte*  start;
      T1_Int    n;

      /* read each parameter independently */
      cur = base;
      for ( n = 0; n < 4; n++ )
      {
        T1_Long*  result;

        /* skip whitespace */
        while (cur < limit && *cur == ' ') cur++;

        /* skip numbers */
        start = cur;
        while (cur < limit && *cur != ' ') cur++;

        /* compute result address */
        switch (n)
        {
          case 0 : result = &matrix->xx; break;
          case 1 : result = &matrix->xy; break;
          case 2 : result = &matrix->yx; break;
          default: result = &matrix->yy;
        }

        error = parse_float( start, cur, 65536000, result );
        if (error) goto Fail;
      }
      parser->error = 0;
      return;
    }

  Fail:
    FT_ERROR(( "T1.CopyMatrix : syntax error !\n" ));
    parser->error = T1_Err_Syntax_Error;
  }



  LOCAL_FUNC
  void  CopyArray( T1_Parser*  parser,
                   T1_Byte*    num_elements,
                   T1_Short*   elements,
                   T1_Int      max_elements )
  {
    T1_Token* token = parser->args++;
    T1_Error  error;

    if ( token->kind == tok_array   ||
         token->kind == tok_program )   /* in the case of MinFeature */
    {
      /* get rid of '['/']', or '{'/'}' */
      T1_Byte*  base  = parser->tokenizer->base + token->start + 1;
      T1_Byte*  limit = base + token->len - 2;
      T1_Byte*  cur;
      T1_Byte*  start;
      T1_Int    n;

      /* read each parameter independently */
      cur = base;
      for ( n = 0; n < max_elements; n++ )
      {
        T1_Long  result;

        /* test end of string */
        if (cur >= limit)
          break;

        /* skip whitespace */
        while (cur < limit && *cur == ' ') cur++;

        /* end of list ? */
        if (cur >= limit)
          break;

        /* skip numbers */
        start = cur;
        while (cur < limit && *cur != ' ') cur++;

        error = parse_integer( start, cur, &result );
        if (error) goto Fail;

        *elements ++ = (T1_Short)result;
      }

      if (num_elements)
        *num_elements = (T1_Byte)n;

      parser->error = 0;
      return;
    }

  Fail:
    FT_ERROR(( "T1.CopyArray : syntax error !\n" ));
    parser->error = T1_Err_Syntax_Error;
  }

