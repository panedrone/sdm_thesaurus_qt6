select 
rg_part_of_speech,
rg_order,
rgw.* from RELATED_GROUPS rg inner join RELATED_GROUP_WORDS rgw on rg.rg_id = rgw.rg_id
where rg.w_id = ?
order by rg_id, rg.rg_order, rgw.rgw_order