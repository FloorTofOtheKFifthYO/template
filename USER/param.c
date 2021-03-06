#include "param.h"
#include "flash.h"
#include <stdlib.h>

/**
  * @brief  初始化参数结构体
  *
  * @param  param: 参数指针  
  *          
  * @retval int: 返回0，出错，程序直接退出
  *				返回1，初始化成功
  */
int param_init(Param * param)
{
	u32 addr = PARAM_FLASH_ADDR_START;
	int i, j, k;
	Pos_data * pos_data;
	Launch_data * launch_data;
	param = (Param *) malloc(sizeof(Param));
	list_init(&param->pos_ptr);
#if FIRST_RUN
	param->pos_num = 0;
#else 
	param->pos_num = STMFLASH_ReadWord_Inc(&addr);
	for (i = 0; i < param->pos_num; ++i)
	{
		pos_data = (Pos_data *) malloc(sizeof(Pos_data));
		if (pos_data == NULL)
			return 0;
		pos_data->x = STMFLASH_ReadFloat_Inc(&addr);
		pos_data->y = STMFLASH_ReadFloat_Inc(&addr);
		for (j = 0; j < 7; ++j)
		{
			pos_data->d[i].launch_num = STMFLASH_ReadWord_Inc(&addr);
			if(pos_data == NULL) return 0;
			for (k = 0; k < pos_data->d[i].launch_num; ++k)
			{
				launch_data = (Launch_data *) malloc(sizeof(Launch_data));
				launch_data->pitch = STMFLASH_ReadFloat_Inc(&addr);
				launch_data->roll = STMFLASH_ReadFloat_Inc(&addr);
				launch_data->speed = STMFLASH_ReadFloat_Inc(&addr);
				launch_data->yaw = STMFLASH_ReadFloat_Inc(&addr);
				
				if(!list_insert(&pos_data->d[i].launch_ptr, k+1, launch_data))
					return 0;
			}
		}

		if(list_insert(&param->pos_ptr, i+1, pos_data))
			return 0;
	}
#endif
	return 1;
}

/**
  * @brief  初始化参数结构体
  *
  * @param  param: 参数指针  
  *          
  * @retval int: 返回<0，出错，程序直接退出
  *				返回1，保存
  */
int param_save(Param * param)
{
	u32 addr = PARAM_FLASH_ADDR_START;
	list_node * pos_ptr;
	list_node * launch_ptr;
	Pos_data * pos_data;
	Launch_data * launch_data;
	int i;
	FLASH_Unlock();  //解锁FLASH，写数据
    FLASH_DataCacheCmd(DISABLE);
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
    								FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);  //写FLASH之前，先清除标志
    if(FLASH_EraseSector(PARAM_FLASH_SETOR, VoltageRange_3) != FLASH_COMPLETE){ 
        FLASH_DataCacheCmd(ENABLE); 
        FLASH_Lock();//上锁
        return -1;   //擦写失败，，退出
    }
	
	param->pos_num = list_get_length(&(param->pos_ptr));
    FLASH_ProgramWord(addr,param->pos_num);
    addr+=4;
    pos_ptr = param->pos_ptr->link;
	
	while(pos_ptr!=NULL)
	{
		pos_data = pos_ptr->data;

		FLASH_ProgramFloat(addr, pos_data->x);
		addr+=4;
		FLASH_ProgramFloat(addr, pos_data->y);
		addr+=4;

		for (i = 0; i < 7; ++i)
		{
			pos_data->d[i].launch_num = list_get_length(&(pos_data->d[i].launch_ptr));
			FLASH_ProgramWord(addr, pos_data->d[i].launch_num);
			launch_ptr = pos_data->d[i].launch_ptr->link;

			while(launch_ptr != NULL)
			{
				launch_data = launch_ptr->data;

				FLASH_ProgramFloat(addr, launch_data->pitch);
				addr+=4;
				FLASH_ProgramFloat(addr, launch_data->roll);
				addr+=4;
				FLASH_ProgramFloat(addr, launch_data->speed);
				addr+=4;
				FLASH_ProgramFloat(addr, launch_data->yaw);
				addr+=4;

				launch_ptr = launch_ptr->link;
			}
		}

		pos_ptr = pos_ptr->link;
	}
	 
    FLASH_DataCacheCmd(ENABLE);
    FLASH_Lock();  //  写保护
    if(addr > PARAM_FLASH_ADDR_END){   //如果超过FLASH的存储空间，则报错返回
        return -2;
    }

    return 1;
}

/**
  * @brief  比较两个发射点坐标
  *     
  * @note	在链表中被调用作比较
  *
  * @param  a,b 指向被比较的(void*)data
  *          
  * @retval bool: 是否相同
  */
bool cmp_pos(void *a,void *b){
	Pos_data* p=(Pos_data*)a;
	Pos_data* q=(Pos_data*)b;
	if (p->x==q->x && p->y==q->y)
		return true;
	return false;
}

/**
  * @brief  比较两个发射参数
  *     
  * @note	在链表中被调用作比较
  *
  * @param  a,b: 被比较的(void*)data
  *          
  * @retval bool: 是否相同
  */
bool cmp_launch(void *a,void *b){
	Launch_data* p=(Launch_data*)a;
	Launch_data* q=(Launch_data*)b;
	if (p->pitch==q->pitch &&
		p->roll==q->roll &&
		p->yaw==q->yaw &&
		p->speed==q->speed)
		return true;
	return false;
}

/**
  * @brief  输出发射点坐标
  *     
  * @note	
  *
  * @param  a: 数据的地址指针  
  *          
  * @retval void
  */
void print_pos(void *a){
	Pos_data* p=(Pos_data*)a;
	USART_SendString(bluetooth,"x:%.2f y:%.2f\n",p->x,p->y);
}

/**
  * @brief  输出发射点参数
  *     
  * @note	
  *
  * @param  a: 数据的地址指针  
  *          
  * @retval void
  */
void print_launch(void *a){
	Launch_data* p=(Launch_data*)a;
	USART_SendString(bluetooth,"pitch:%.2f roll:%.2f yaw:%.2f speed:%.2f\n",p->pitch,p->roll,p->yaw,p->speed);
}

/**
  * @brief  输出发射参数链表
  *     
  * @note	
  *
  * @param  p: 第一组发射参数的链表节点指针  
  *          
  * @retval void
  */
void print_launch_list(link_list p){
	while (p!=NULL){
		print_launch(p->data);
		p=p->link;
	}
}

/**
  * @brief  输出发射坐标链表
  *     
  * @note	
  *
  * @param  p: 第一组发射坐标的链表节点指针  
  *          
  * @retval void
  */
void print_pos_list(link_list p){
	while (p!=NULL){
		print_pos(p->data);
		p=p->link;
	}
}

/**
  * @brief  输出整个链表
  *     
  * @note	
  *
  * @param  p: 参数指针  
  *          
  * @retval void
  */
void print_all(Param *p){
	int i;
	Pos_data *pos_temp;
	link_list ptr=p->pos_ptr;
	while (ptr!=NULL){
		print_pos(ptr->data);
		pos_temp=(Pos_data*)(ptr->data);
		for (i=0;i<7;i++)
		if (pos_temp->d[i].launch_num>0)
			print_launch_list(pos_temp->d[i].launch_ptr);
		ptr=ptr->link;
	}
}


