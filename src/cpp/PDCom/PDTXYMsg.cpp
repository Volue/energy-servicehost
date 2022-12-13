/*
Copyright (C) Powel Data AS
All rights reserved.
*/
#include <PrePDCom.h>
#include <PDTXYMsg.h>

using namespace std;

RWDEFINE_NAMED_COLLECTABLE(PDTXYMsg, "PDTXYMsg");

PDTXYMsg::PDTXYMsg()
{
	_txy=NULL;
}
PDTXYMsg::~PDTXYMsg()
{
	_txy=NULL;
}
void
PDTXYMsg::setMsg(void *p)
{
	_txy = (TXY_SERIES *)p;
}
RWspace  PDTXYMsg::binaryStoreSize() const
{
	RWspace count=0;
	if (!_txy) return count;
	count = RWCollectable::binaryStoreSize() + TXY_get_size(_txy->num_values);
	return count;
}
void PDTXYMsg::saveGuts(RWFile& o) const
{
	PDCollectable::saveGuts(o);
	if(!_txy)return;
	txyGuts(o,<<,;,1);
}
void PDTXYMsg::saveGuts(RWvostream& o) const
{
	PDCollectable::saveGuts(o);
	if(!_txy)return;
	txyGuts(o,<<,;,1);
}
void PDTXYMsg::restoreGuts(RWFile& i)
{
	PDCollectable::restoreGuts(i);
	if (!_txy)
	{
		_txy = TXY_create(1);
	}
	txyGuts(i,>>,_txy=TXY_realloc(_txy, num_values),0);
}
void PDTXYMsg::restoreGuts(RWvistream& i)
{
	PDCollectable::restoreGuts(i);
	if (!_txy)
	{
		_txy = TXY_create(1);
	}
	txyGuts(i,>>,_txy=TXY_realloc(_txy, num_values),0);
}
void*
PDTXYMsg::generateTestGuts(int numValues)
{
	TXY_SERIES *test_txy= TXY_create(numValues);
	test_txy->tag=1;
	test_txy->tag_type=2;
	test_txy->client_data=3;
	test_txy->kind=4;
	strcpy(test_txy->name,"name5");
	strcpy(test_txy->text,"text6");
	strcpy(test_txy->start_time,"199707070707000");
	strcpy(test_txy->__padding_6__,"pad_6");
	strcpy(test_txy->__padding_18__,"pad_18");
	test_txy->id=10;
	test_txy->group=11;
	test_txy->x_unit=HOUR;
	test_txy->x_step=1;
	test_txy->ref=0.0;
	test_txy->val_unit=MWH;
	test_txy->data_type=10;
	test_txy->number_of_missing_data=0;
	test_txy->update=0;
	test_txy->period=0;
	test_txy->number=0;
	test_txy->flags=10;
	test_txy->max_y=1000;
	test_txy->min_y=0;
	test_txy->origin=12;
	test_txy->producer=11;
	test_txy->vlcode=2;
	strcpy(test_txy->x_format,"x format");
	strcpy(test_txy->y_format,"y format");
	int j;
	for (j = 0; j < test_txy->num_values; j++)
	{
		test_txy->values[j] = j+100.0;
		TXY_STATUS(test_txy,j)=1;
	}
	return test_txy;
}
void PDTXYMsg::printGuts(void *p,int numValues)
{
#ifdef PDCOM_TEST
	TXY_SERIES *txy = (TXY_SERIES *)p;
	cout << "tag:"<< txy->tag << endl;
	cout << "tag_type:" << txy->tag_type <<endl;
	cout << "client_data:" << txy->client_data <<endl;
	cout << "kind:" << txy->kind << endl;
	cout << "name:" << txy->name << endl;
	cout << "text:" << txy->text << endl;
	cout << "start_time:"  << txy->start_time << endl;
	cout << "__padding_6__:"  << txy->__padding_6__ << endl;
	cout << "__padding_18__:"  << txy->__padding_18__ << endl;
	cout << "id:"  << txy->id << endl;
	cout << "group:"  << txy->group << endl;
	cout << "x_unit:"  << txy->x_unit << endl;
	cout << "x_step:"  << txy->x_step << endl;
	cout << "ref:"  << txy->ref << endl;
	cout << "val_unit:"  << txy->val_unit << endl;
	cout << "data_type:"  << txy->data_type << endl;
	cout << "number_of_missing_data:"  << txy->number_of_missing_data << endl;
	cout << "update:"  << txy->update << endl;
	cout << "period:"  << txy->period << endl;
	cout << "number:"  << txy->number << endl;
	cout << "flags:"  << txy->flags << endl;
	cout << "max_y:"  << txy->max_y << endl;
	cout << "min_y:"  << txy->min_y << endl;
	cout << "origin:"  << txy->origin << endl;
	cout << "producer:"  << txy->producer << endl;
	cout << "vlcode:"  << txy->vlcode << endl;
	cout << "x_format:"  << txy->x_format << endl;
	cout << "y_format:"  << txy->y_format << endl;
	cout << "num_values:"  << txy->num_values << endl;
	int j;
	int num;
	num=numValues;
	if (num > txy->num_values)num=txy->num_values;
	for (j = 0; j < txy->num_values; j++)
	{
		cout << "value[" << j <<"] = " << txy->values[j];
		cout << "  STATUS[" << j <<"] = " <<  TXY_STATUS(txy,j) << endl;
	}
#endif
}
