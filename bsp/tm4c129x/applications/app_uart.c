#include "app_reg.h"
struct rt_semaphore rx_sem;
rt_device_t uart_dev = RT_NULL;
void uart_thread_entry(void* parameter);
static struct rt_thread uart_thread;
ALIGN(RT_ALIGN_SIZE)
static char uart_thread_stack[2048];

static rt_err_t uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    /* release semaphore to let finsh thread rx data */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

int app_uart_init()
{
	/*init uart device*/
	rt_err_t result;
	uart_dev = rt_device_find("uart1");
	if (uart_dev == RT_NULL)
	{
	  rt_kprintf("app_uart: can not find device: %s\n", "uart1");
	  return 0;
	}
	if (rt_device_open(uart_dev, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX) == RT_EOK)
	{
	  rt_device_set_rx_indicate(uart_dev, uart_rx_ind);
	}
	rt_sem_init(&rx_sem, "uart1_rx", 0, 0);
	result = rt_thread_init(&uart_thread,
        "uart_rx",
        uart_thread_entry, RT_NULL,
        &uart_thread_stack[0], sizeof(uart_thread_stack),
        20, 10);

    if (result == RT_EOK)
        rt_thread_startup(&uart_thread);
    return 1;
}
void uart_thread_entry(void* parameter)
{
    char ch;

    while (1)
    {
        /* wait receive */
        if (rt_sem_take(&rx_sem, RT_WAITING_FOREVER) != RT_EOK) continue;

        /* read one character from device */
        while (rt_device_read(uart_dev, 0, &ch, 1) == 1)
        {
            /*
             * handle control key
             * up key  : 0x1b 0x5b 0x41
             * down key: 0x1b 0x5b 0x42
             * right key:0x1b 0x5b 0x43
             * left key: 0x1b 0x5b 0x44
             */
             	rt_kprintf("%c",ch);
             #if 0
            if (ch == 0x1b)
            {
                shell->stat = WAIT_SPEC_KEY;
                continue;
            }
            else if (shell->stat == WAIT_SPEC_KEY)
            {
                if (ch == 0x5b)
                {
                    shell->stat = WAIT_FUNC_KEY;
                    continue;
                }

                shell->stat = WAIT_NORMAL;
            }
            else if (shell->stat == WAIT_FUNC_KEY)
            {
                shell->stat = WAIT_NORMAL;

                if (ch == 0x41) /* up key */
                {
#ifdef FINSH_USING_HISTORY
                    /* prev history */
                    if (shell->current_history > 0)
                        shell->current_history --;
                    else
                    {
                        shell->current_history = 0;
                        continue;
                    }

                    /* copy the history command */
                    memcpy(shell->line, &shell->cmd_history[shell->current_history][0],
                           FINSH_CMD_SIZE);
                    shell->line_curpos = shell->line_position = strlen(shell->line);
                    shell_handle_history(shell);
#endif
                    continue;
                }
                else if (ch == 0x42) /* down key */
                {
#ifdef FINSH_USING_HISTORY
                    /* next history */
                    if (shell->current_history < shell->history_count - 1)
                        shell->current_history ++;
                    else
                    {
                        /* set to the end of history */
                        if (shell->history_count != 0)
                            shell->current_history = shell->history_count - 1;
                        else
                            continue;
                    }

                    memcpy(shell->line, &shell->cmd_history[shell->current_history][0],
                           FINSH_CMD_SIZE);
                    shell->line_curpos = shell->line_position = strlen(shell->line);
                    shell_handle_history(shell);
#endif
                    continue;
                }
                else if (ch == 0x44) /* left key */
                {
                    if (shell->line_curpos)
                    {
                        rt_kprintf("\b");
                        shell->line_curpos --;
                    }

                    continue;
                }
                else if (ch == 0x43) /* right key */
                {
                    if (shell->line_curpos < shell->line_position)
                    {
                        rt_kprintf("%c", shell->line[shell->line_curpos]);
                        shell->line_curpos ++;
                    }

                    continue;
                }

            }

            /* handle CR key */
            if (ch == '\r')
            {
                char next;

                if (rt_device_read(shell->device, 0, &next, 1) == 1)
                    ch = next;
                else ch = '\r';
            }
            /* handle tab key */
            else if (ch == '\t')
            {
                int i;
                /* move the cursor to the beginning of line */
                for (i = 0; i < shell->line_curpos; i++)
                    rt_kprintf("\b");

                /* auto complete */
                shell_auto_complete(&shell->line[0]);
                /* re-calculate position */
                shell->line_curpos = shell->line_position = strlen(shell->line);

                continue;
            }
            /* handle backspace key */
            else if (ch == 0x7f || ch == 0x08)
            {
                /* note that shell->line_curpos >= 0 */
                if (shell->line_curpos == 0)
                    continue;

                shell->line_position--;
                shell->line_curpos--;

                if (shell->line_position > shell->line_curpos)
                {
                    int i;

                    rt_memmove(&shell->line[shell->line_curpos],
                               &shell->line[shell->line_curpos + 1],
                               shell->line_position - shell->line_curpos);
                    shell->line[shell->line_position] = 0;

                    rt_kprintf("\b%s  \b", &shell->line[shell->line_curpos]);

                    /* move the cursor to the origin position */
                    for (i = shell->line_curpos; i <= shell->line_position; i++)
                        rt_kprintf("\b");
                }
                else
                {
                    rt_kprintf("\b \b");
                    shell->line[shell->line_position] = 0;
                }

                continue;
            }

            /* handle end of line, break */
            if (ch == '\r' || ch == '\n')
            {
                #ifdef FINSH_USING_HISTORY
                shell_push_history(shell);
                #endif

                #ifdef FINSH_USING_MSH
                if (msh_is_used() == RT_TRUE)
                {
                    rt_kprintf("\n");
                    msh_exec(shell->line, shell->line_position);
                }
                else
                #endif
                {
                #ifndef FINSH_USING_MSH_ONLY                
                    /* add ';' and run the command line */
                    shell->line[shell->line_position] = ';';

                    if (shell->line_position != 0) finsh_run_line(&shell->parser, shell->line);
                    else rt_kprintf("\n");
                #endif                  
                }

                rt_kprintf(FINSH_PROMPT);
                memset(shell->line, 0, sizeof(shell->line));
                shell->line_curpos = shell->line_position = 0;
                break;
            }

            /* it's a large line, discard it */
            if (shell->line_position >= FINSH_CMD_SIZE)
                shell->line_position = 0;

            /* normal character */
            if (shell->line_curpos < shell->line_position)
            {
                int i;

                rt_memmove(&shell->line[shell->line_curpos + 1],
                           &shell->line[shell->line_curpos],
                           shell->line_position - shell->line_curpos);
                shell->line[shell->line_curpos] = ch;
                if (shell->echo_mode)
                    rt_kprintf("%s", &shell->line[shell->line_curpos]);

                /* move the cursor to new position */
                for (i = shell->line_curpos; i < shell->line_position; i++)
                    rt_kprintf("\b");
            }
            else
            {
                shell->line[shell->line_position] = ch;
                rt_kprintf("%c", ch);
            }

            ch = 0;
            shell->line_position ++;
            shell->line_curpos++;
			if (shell->line_position >= 80) 
			{
				/* clear command line */
				shell->line_position = 0;
				shell->line_curpos = 0;
			}
			#endif
        } /* end of device read */
    }
}

